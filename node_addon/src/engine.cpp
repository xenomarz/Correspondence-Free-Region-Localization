// STL includes
#include <sstream>

// LIBIGL includes
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

// Optimization lib includes
#include "../include/engine.h"

Napi::FunctionReference Engine::constructor;

Napi::Object Engine::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Engine", {
		InstanceMethod("loadModel", &Engine::LoadModel),
		InstanceMethod("resumeSolver", &Engine::ResumeSolver),
		InstanceMethod("pauseSolver", &Engine::PauseSolver),
		InstanceMethod("constrainFacePosition", &Engine::ConstrainFacePosition),
		InstanceMethod("updateConstrainedFacePosition", &Engine::UpdateConstrainedFacePosition),
		InstanceMethod("unconstrainFacePosition", &Engine::UnconstrainFacePosition),
		InstanceMethod("reconstrainFacePosition", &Engine::ReconstrainFacePosition),
		InstanceMethod("getDomainFacesCount", &Engine::GetDomainFacesCount),
		InstanceMethod("getImageFacesCount", &Engine::GetImageFacesCount),
		InstanceMethod("getDomainVerticesCount", &Engine::GetDomainVerticesCount),
		InstanceMethod("getImageVerticesCount", &Engine::GetImageVerticesCount),
		InstanceMethod("getDomainFaces", &Engine::GetDomainFaces),
		InstanceMethod("getImageFaces", &Engine::GetImageFaces),
		InstanceMethod("getDomainVertices", &Engine::GetDomainVertices),
		InstanceMethod("getImageVertices", &Engine::GetImageVertices),
		InstanceMethod("getDomainBufferedVertices", &Engine::GetDomainBufferedVertices),
		InstanceMethod("getImageBufferedVertices", &Engine::GetImageBufferedVertices),
		InstanceMethod("getDomainBufferedUvs", &Engine::GetDomainBufferedUvs),
		InstanceMethod("getImageBufferedUvs", &Engine::GetImageBufferedUvs),
		InstanceAccessor("positionWeight", &Engine::GetPositionWeight, &Engine::SetPositionWeight),
		InstanceAccessor("seamlessWeight", &Engine::GetSeamlessWeight, &Engine::SetSeamlessWeight),
		InstanceAccessor("lambda", &Engine::GetLambda, &Engine::SetLambda),
		InstanceAccessor("delta", &Engine::GetDelta, &Engine::SetDelta),
		InstanceAccessor("objectiveFunctionsData", &Engine::GetObjectiveFunctionsData, nullptr)
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Engine", func);
	return exports;
}

Engine::Engine(const Napi::CallbackInfo& info) : 
	Napi::ObjectWrap<Engine>(info),
	mesh_wrapper_(std::make_shared<MeshWrapper>())
{
	position_ = std::make_shared<Position>(mesh_wrapper_);
	separation_ = std::make_shared<Separation>(mesh_wrapper_);
	symmetric_dirichlet_ = std::make_shared<SymmetricDirichlet>(mesh_wrapper_);
	position_ = std::make_shared<Position>(mesh_wrapper_);
	std::vector<std::shared_ptr<ObjectiveFunction>> objective_functions;
	objective_functions.push_back(position_);
	objective_functions.push_back(separation_);
	objective_functions.push_back(symmetric_dirichlet_);
	composite_objective_ = std::make_shared<CompositeObjective>(mesh_wrapper_, objective_functions);
	mesh_wrapper_->RegisterModelLoadedCallback([&]() {
		/**
		 * Initialize objective functions
		 */
		composite_objective_->Initialize();

		/**
		 * Create newton method iterator
		 */
		auto image_vertices = mesh_wrapper_->GetImageVertices();
		auto x0 = Eigen::Map<const Eigen::VectorXd>(image_vertices.data(), image_vertices.cols() * image_vertices.rows());
		newton_method_ = std::make_unique<NewtonMethod<PardisoSolver, Eigen::StorageOptions::RowMajor>>(composite_objective_, x0);
		newton_method_->EnableFlipAvoidingLineSearch(mesh_wrapper_->GetImageFaces());
	});
}


Napi::Value Engine::GetDomainVerticesCount(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number count = Napi::Number::New(env, mesh_wrapper_->GetDomainVertices().rows());

	return count;
}

Napi::Value Engine::GetImageVerticesCount(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number count = Napi::Number::New(env, mesh_wrapper_->GetImageVertices().rows());

	return count;
}

Napi::Value Engine::GetDomainFacesCount(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number count = Napi::Number::New(env, mesh_wrapper_->GetDomainFaces().rows());

	return count;
}

Napi::Value Engine::GetImageFacesCount(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number count = Napi::Number::New(env, mesh_wrapper_->GetImageVertices().rows());

	return count;
}

Napi::Value Engine::GetDomainFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, mesh_wrapper_->GetDomainFaces());
}

Napi::Value Engine::GetImageFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, mesh_wrapper_->GetImageFaces());
}

Napi::Value Engine::GetDomainVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVerticesArray(env, mesh_wrapper_->GetDomainVertices());
}

Napi::Value Engine::GetImageVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	TryUpdateImageVertices();

	return CreateVerticesArray(env, mesh_wrapper_->GetImageVertices());
}

Napi::Value Engine::GetDomainBufferedVertices(const Napi::CallbackInfo& info)
{
	TryUpdateImageVertices();
	return GetBufferedVertices(info, VerticesSource::DOMAIN_VERTICES);
}

Napi::Value Engine::GetImageBufferedVertices(const Napi::CallbackInfo& info)
{
	TryUpdateImageVertices();
	return GetBufferedVertices(info, VerticesSource::IMAGE_VERTICES);
}

Napi::Value Engine::GetDomainBufferedUvs(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	TryUpdateImageVertices();

	return CreateBufferedUvsArray(env, mesh_wrapper_->GetImageVertices(), mesh_wrapper_->GetImageFaces());
}

Napi::Value Engine::GetImageBufferedUvs(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	TryUpdateImageVertices();

	return CreateBufferedUvsArray(env, mesh_wrapper_->GetImageVertices(), mesh_wrapper_->GetImageFaces());
}

Napi::Float32Array Engine::GetBufferedVertices(const Napi::CallbackInfo& info, const VerticesSource vertices_source)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 1)
	{
		if (!info[0].IsNumber())
		{
			Napi::TypeError::New(env, "First argument is expected to be a Number").ThrowAsJavaScriptException();
			return Napi::Float32Array::New(env, 0);
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Float32Array::New(env, 0);
	}

	/**
	 * Create buffered vertices array according to the provided primitive type
	 */
	Napi::Number argument1 = info[0].As<Napi::Number>();
	BufferedPrimitiveType buffered_primitive_type = static_cast<BufferedPrimitiveType>(argument1.Uint32Value());

	switch (buffered_primitive_type)
	{
	case BufferedPrimitiveType::VERTEX:
		switch (vertices_source)
		{
		case VerticesSource::DOMAIN_VERTICES:
			return CreateBufferedVerticesArray(env, mesh_wrapper_->GetDomainVertices());
		case VerticesSource::IMAGE_VERTICES:
			return CreateBufferedVerticesArray(env, mesh_wrapper_->GetImageVertices());
		}
		break;

	case BufferedPrimitiveType::TRIANGLE:
		switch (vertices_source)
		{
		case VerticesSource::DOMAIN_VERTICES:
			return CreateBufferedVerticesArray(env, mesh_wrapper_->GetDomainVertices(), mesh_wrapper_->GetDomainFaces());
		case VerticesSource::IMAGE_VERTICES:
			return CreateBufferedVerticesArray(env, mesh_wrapper_->GetImageVertices(), mesh_wrapper_->GetImageFaces());
		}
		break;
	}

	Napi::TypeError::New(env, "Unknown buffered primitive type").ThrowAsJavaScriptException();
	return Napi::Float32Array::New(env, 0);
}

Napi::Value Engine::LoadModel(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() <= 0 || !info[0].IsString())
	{
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Load model
	 */
	Napi::String value = info[0].As<Napi::String>();
	std::string model_file_path = std::string(value);
	mesh_wrapper_->LoadModel(model_file_path);

	return env.Null();
}

Engine::ModelFileType Engine::GetModelFileType(std::string modelFilePath)
{
	std::string fileExtension = modelFilePath.substr(modelFilePath.find_last_of(".") + 1);
	transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
	if (fileExtension == "obj")
	{
		return Engine::ModelFileType::OBJ;
	}
	else if (fileExtension == "off")
	{
		return Engine::ModelFileType::OFF;
	}

	return Engine::ModelFileType::UNKNOWN;
}

Napi::Array Engine::CreateFaces(Napi::Env env, const Eigen::MatrixX3i& F)
{
	Napi::Array faces_array = Napi::Array::New(env, F.rows());
	for (int face_index = 0; face_index < F.rows(); face_index++)
	{
		Napi::Object face_object = Napi::Object::New(env);
		int v0 = F(face_index, 0);
		int v1 = F(face_index, 1);
		int v2 = F(face_index, 2);

		face_object.Set("v0", v0);
		face_object.Set("v1", v1);
		face_object.Set("v2", v2);
		faces_array[face_index] = face_object;
	}

	return faces_array;
}

void Engine::TryUpdateImageVertices()
{
	Eigen::VectorXd approximation_vector;
	if (newton_method_->GetApproximation(approximation_vector))
	{
		auto image_vertices = Eigen::Map<const Eigen::MatrixX2d>(approximation_vector.data(), approximation_vector.rows() >> 1, 2);
		mesh_wrapper_->SetImageVertices(image_vertices);
	}
}

void Engine::SetPositionWeight(const Napi::CallbackInfo& info, const Napi::Value& value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (!value.IsNumber())
	{
		Napi::TypeError::New(env, "value is expected to be a Number").ThrowAsJavaScriptException();
		return;
	}

	/**
	 * Set position weight
	 */
	Napi::Number number = value.As<Napi::Number>();
	double position_weight = number.DoubleValue();
	if (position_) {
		position_->SetWeight(position_weight);
	}
}

Napi::Value Engine::GetPositionWeight(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number position_weight = Napi::Number::New(env, position_->GetWeight());

	return position_weight;
}

void Engine::SetSeamlessWeight(const Napi::CallbackInfo& info, const Napi::Value& value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (!value.IsNumber())
	{
		Napi::TypeError::New(env, "value is expected to be a Number").ThrowAsJavaScriptException();
		return;
	}

	/**
	 * Set seamless weight
	 */
	Napi::Number number = value.As<Napi::Number>();
	double seamless_weight = number.DoubleValue();
}

Napi::Value Engine::GetSeamlessWeight(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return env.Null();
}

void Engine::SetLambda(const Napi::CallbackInfo& info, const Napi::Value& value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (!value.IsNumber())
	{
		Napi::TypeError::New(env, "value is expected to be a Number").ThrowAsJavaScriptException();
		return;
	}

	/**
	 * Set lambda
	 */
	Napi::Number number = value.As<Napi::Number>();
	double lambda = number.DoubleValue();

	separation_->SetWeight(lambda);
	symmetric_dirichlet_->SetWeight(1.0 - lambda);
}

Napi::Value Engine::GetLambda(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number lambda = Napi::Number::New(env, separation_->GetWeight());

	return lambda;
}

void Engine::SetDelta(const Napi::CallbackInfo& info, const Napi::Value& value)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (!value.IsNumber())
	{
		Napi::TypeError::New(env, "value is expected to be a Number").ThrowAsJavaScriptException();
		return;
	}

	/**
	 * Set delta
	 */
	Napi::Number number = value.As<Napi::Number>();
	double delta = number.DoubleValue();
	separation_->SetDelta(delta);
}

Napi::Value Engine::GetDelta(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number delta = Napi::Number::New(env, separation_->GetDelta());

	return delta;
}

Napi::Value Engine::GetObjectiveFunctionsData(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Array objective_functions_data_array = Napi::Array::New(env);
	for (std::uint32_t index = 0; index < composite_objective_->GetObjectiveFunctionsCount(); index++)
	{
		Napi::Object data_object = Napi::Object::New(env);
		Napi::Object data_object_internal = Napi::Object::New(env);

		auto current_objective_function = composite_objective_->GetObjectiveFunction(index);
		data_object.Set("name", current_objective_function->GetName());
		data_object.Set("data", data_object_internal);
		data_object_internal.Set("value", current_objective_function->GetValue());
		data_object_internal.Set("gradientNorm", current_objective_function->GetGradient().norm());
		objective_functions_data_array[index] = data_object;
	}

	return objective_functions_data_array;
}

Napi::Value Engine::ResumeSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (newton_method_) {
		newton_method_->Resume();
	}

	return env.Null();
}

Napi::Value Engine::PauseSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (newton_method_) {
		newton_method_->Pause();
	}

	return env.Null();
}

Napi::Value Engine::ConstrainFacePosition(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 1)
	{
		if (!info[0].IsNumber())
		{
			Napi::TypeError::New(env, "First argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Add face vertices to the constrained vertices list of the position objective function
	 */
	Napi::Number argument1 = info[0].As<Napi::Number>();
	Eigen::DenseIndex face_index = argument1.Int64Value();
	Eigen::VectorXi face_vertices_indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);
	Eigen::MatrixXd face_vertices = mesh_wrapper_->GetImageVertices(face_vertices_indices);

	if (position_)
	{
		std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>> index_position_pairs;
		for (int i = 0; i < 3; i++)
		{
			index_position_pairs.push_back(std::make_pair(face_vertices_indices(i), face_vertices.row(i)));
		}

		position_->AddConstrainedVertices(index_position_pairs);
	}

	return env.Null();
}

Napi::Value Engine::UpdateConstrainedFacePosition(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 3)
	{
		if (!info[0].IsNumber())
		{
			Napi::TypeError::New(env, "First argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}

		if (!info[1].IsNumber())
		{
			Napi::TypeError::New(env, "Second argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}

		if (!info[2].IsNumber())
		{
			Napi::TypeError::New(env, "Third argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Move the position of the constrained face vertices by a given offset
	 */
	Napi::Number argument1 = info[0].As<Napi::Number>();
	Napi::Number argument2 = info[1].As<Napi::Number>();
	Napi::Number argument3 = info[2].As<Napi::Number>();
	Eigen::DenseIndex face_index = argument1.Int64Value();
	double offset_x = argument2.DoubleValue();
	double offset_y = argument3.DoubleValue();
	Eigen::Vector2d offset = Eigen::Vector2d(offset_x, offset_y);
	Eigen::VectorXi face_vertices_indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);

	if (position_)
	{
		std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>> index_offset_pairs;
		for (int i = 0; i < 3; i++)
		{
			index_offset_pairs.push_back(std::make_pair(face_vertices_indices(i), offset));
		}

		position_->OffsetConstrainedVerticesPositions(index_offset_pairs);
	}

	return env.Null();
}

Napi::Value Engine::UnconstrainFacePosition(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 1)
	{
		if (!info[0].IsNumber())
		{
			Napi::TypeError::New(env, "First argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Remove face vertices from the constrained vertices list of the position objective function
	 */
	Napi::Number argument1 = info[0].As<Napi::Number>();
	Eigen::DenseIndex face_index = argument1.Int64Value();
	Eigen::VectorXi face_vertices_indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);

	if (position_)
	{
		std::vector<Eigen::DenseIndex> vertices_indices;
		for (int i = 0; i < 3; i++)
		{
			vertices_indices.push_back(face_vertices_indices(i));
		}

		position_->RemoveConstrainedVertices(vertices_indices);
	}

	return env.Null();
}

Napi::Value Engine::ReconstrainFacePosition(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 1)
	{
		if (!info[0].IsNumber())
		{
			Napi::TypeError::New(env, "First argument is expected to be a Number").ThrowAsJavaScriptException();
			return env.Null();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Remove face vertices from the constrained vertices list of the position objective function
	 */
	Napi::Number argument1 = info[0].As<Napi::Number>();
	Eigen::DenseIndex face_index = argument1.Int64Value();
	Eigen::VectorXi face_vertices_indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);

	if (position_)
	{
		std::vector<Eigen::DenseIndex> vertices_indices;
		for (int i = 0; i < 3; i++)
		{
			vertices_indices.push_back(face_vertices_indices(i));
		}

		position_->ResetConstrainedVertices(vertices_indices);
	}

	return env.Null();
}