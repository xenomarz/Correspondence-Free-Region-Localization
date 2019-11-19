// STL includes
#include <sstream>
#include <any>

// LIBIGL includes
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

// Optimization lib includes
#include "../include/engine.h"
#include "libs/optimization_lib/include/objective_functions/objective_function.h"
#include "libs/optimization_lib/include/objective_functions/dense_objective_function.h"
#include "libs/optimization_lib/include/objective_functions/position/barycenter_position_objective.h"
#include "libs/optimization_lib/include/objective_functions/position/vertex_position_objective.h"
#include "libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_angle_objective.h"
#include "libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_length_objective.h"
#include "libs/optimization_lib/include/objective_functions/singularity/singular_points_objective.h"

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
		InstanceMethod("getDomainFacesCount", &Engine::GetDomainFacesCount),
		InstanceMethod("getImageFacesCount", &Engine::GetImageFacesCount),
		InstanceMethod("getDomainVerticesCount", &Engine::GetDomainVerticesCount),
		InstanceMethod("getImageVerticesCount", &Engine::GetImageVerticesCount),
		InstanceMethod("getDomainFaces", &Engine::GetDomainFaces),
		InstanceMethod("getImageFaces", &Engine::GetImageFaces),
		InstanceMethod("getDomainVertices", &Engine::GetDomainVertices),
		InstanceMethod("getImageVertices", &Engine::GetImageVertices),
		InstanceMethod("getDomainBufferedFaces", &Engine::GetDomainBufferedFaces),
		InstanceMethod("getImageBufferedFaces", &Engine::GetImageBufferedFaces),
		InstanceMethod("getDomainBufferedVertices", &Engine::GetDomainBufferedVertices),
		InstanceMethod("getImageBufferedVertices", &Engine::GetImageBufferedVertices),
		InstanceMethod("getDomainBufferedUvs", &Engine::GetDomainBufferedUvs),
		InstanceMethod("getImageBufferedUvs", &Engine::GetImageBufferedUvs),
		InstanceMethod("getObjectiveFunctionProperty", &Engine::GetObjectiveFunctionProperty),
		InstanceMethod("setObjectiveFunctionProperty", &Engine::SetObjectiveFunctionProperty),
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
	properties_map_.insert({ "value", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::Value) });
	properties_map_.insert({ "value_per_vertex", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::ValuePerVertex) });
	properties_map_.insert({ "gradient", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::Gradient) });
	properties_map_.insert({ "gradient_norm", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::GradientNorm) });
	properties_map_.insert({ "hessian", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::Hessian) });
	properties_map_.insert({ "weight", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::Weight) });
	properties_map_.insert({ "name", static_cast<uint32_t>(DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>::Properties::Name) });
	properties_map_.insert({ "delta", static_cast<uint32_t>(Separation<Eigen::StorageOptions::RowMajor>::Properties::Delta) });
	properties_map_.insert({ "interval", static_cast<uint32_t>(SingularPointsObjective<Eigen::StorageOptions::RowMajor>::Properties::Interval) });

	plain_data_provider_ = std::make_shared<PlainDataProvider>(mesh_wrapper_);
	for(auto& edge_pair_descriptor : mesh_wrapper_->GetEdgePairDescriptors())
	{
		edge_pair_data_providers_.push_back(std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptor));
	}

	for (auto& face_fan : mesh_wrapper_->GetFaceFans())
	{
		face_fan_data_providers_.push_back(std::make_shared<FaceFanDataProvider>(mesh_wrapper_, face_fan));
	}
	
	// TODO: Expose interface for addition and removal of objective function
	separation_ = std::make_shared<Separation<Eigen::StorageOptions::RowMajor>>(plain_data_provider_);
	symmetric_dirichlet_ = std::make_shared<SymmetricDirichlet<Eigen::StorageOptions::RowMajor>>(plain_data_provider_);
	seamless_ = std::make_shared<SeamlessObjective<Eigen::StorageOptions::RowMajor>>();
	singular_points_ = std::make_shared<SingularPointsObjective<Eigen::StorageOptions::RowMajor>>(1);
  	position_ = std::make_shared<SummationObjective<DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>>>(std::string("Position"));
	std::vector<std::shared_ptr<DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>>> objective_functions;
	objective_functions.push_back(position_);
	objective_functions.push_back(separation_);
	objective_functions.push_back(symmetric_dirichlet_);
	objective_functions.push_back(seamless_);
	objective_functions.push_back(singular_points_);
	summation_objective_ = std::make_shared<SummationObjective<DenseObjectiveFunction<Eigen::StorageOptions::RowMajor>>>(objective_functions, false, true);
	mesh_wrapper_->RegisterModelLoadedCallback([&]() {
		/**
		 * Initialize objective functions
		 */
		summation_objective_->Initialize();

		for (auto& edge_pair_data_provider : edge_pair_data_providers_)
		{
			seamless_->AddEdgePairObjectives(edge_pair_data_provider);
		}

		for (auto& face_fan_data_provider : face_fan_data_providers_)
		{
			singular_points_->AddSingularPointObjective(face_fan_data_provider);
		}

		//auto& dom_v_2_im_v_map = mesh_wrapper_->GetDomainVerticesToImageVerticesMap();
		//for(int64_t i = 0; i < mesh_wrapper_->GetDomainVerticesCount(); i++)
		//{
		//	std::vector<SingularityObjective<Eigen::StorageOptions::RowMajor>::SingularCorner> singular_corners;
		//	auto& image_indices = dom_v_2_im_v_map.at(i);
		//	for(int64_t corner_index = 0; corner_index < image_indices.size(); corner_index++)
		//	{
		//		auto image_index = image_indices[corner_index];
		//		auto& neighbours = mesh_wrapper_->GetImageNeighbours().at(image_index);
		//		singular_corners.push_back(std::make_pair(image_index, neighbours));
		//	}

		//	singularity_->AddSingularCorners(singular_corners);
		//}
		//singularity_->AddSingularCornersTest();


		/**
		 * Create newton method iterator
		 */
		auto image_vertices = mesh_wrapper_->GetImageVertices();
		auto x0 = Eigen::Map<const Eigen::VectorXd>(image_vertices.data(), image_vertices.cols() * image_vertices.rows());
		newton_method_ = std::make_unique<NewtonMethod<PardisoSolver, Eigen::StorageOptions::RowMajor>>(summation_objective_, x0);
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

Napi::Value Engine::GetDomainBufferedFaces(const Napi::CallbackInfo& info)
{
	return GetBufferedFaces(info, FacesSource::DOMAIN_FACES);
}

Napi::Value Engine::GetImageBufferedFaces(const Napi::CallbackInfo& info)
{
	return GetBufferedFaces(info, FacesSource::IMAGE_FACES);
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

Napi::Int32Array Engine::GetBufferedFaces(const Napi::CallbackInfo& info, const FacesSource faces_source) const
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Create buffered faces array
	 */
	switch (faces_source)
	{
	case FacesSource::DOMAIN_FACES:
		return CreateBufferedFacesArray(env, mesh_wrapper_->GetDomainFaces());
	case FacesSource::IMAGE_FACES:
		return CreateBufferedFacesArray(env, mesh_wrapper_->GetImageFaces());
	}

	Napi::TypeError::New(env, "Unknown buffered primitive type").ThrowAsJavaScriptException();
	return Napi::Int32Array::New(env, 0);
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

Napi::Value Engine::GetObjectiveFunctionProperty(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 2)
	{
		if (!info[0].IsString())
		{
			Napi::TypeError::New(env, "First argument is expected to be a String").ThrowAsJavaScriptException();
			return Napi::Value();
		}

		if (!info[1].IsString())
		{
			Napi::TypeError::New(env, "Second argument is expected to be a String").ThrowAsJavaScriptException();
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Get objective function by name
	 */
	const auto objective_function = summation_objective_->GetObjectiveFunction(info[0].ToString());
	if(objective_function == nullptr)
	{
		Napi::TypeError::New(env, "Objective function could not be found").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Get property
	 */
	const std::string property_name = info[1].ToString();
	if(properties_map_.contains(property_name))
	{
		const uint32_t property_id = properties_map_.at(property_name);
		std::any any_value;
		if (objective_function->GetProperty(property_id, any_value))
		{
			return NativeToJS(env, any_value);
		}
		
		Napi::TypeError::New(env, "Couldn't get property").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Property not found
	 */
	Napi::TypeError::New(env, "Property name could not be found").ThrowAsJavaScriptException();
	return Napi::Value();
}

Napi::Value Engine::SetObjectiveFunctionProperty(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 3)
	{
		if (!info[0].IsString())
		{
			Napi::TypeError::New(env, "First argument is expected to be a String").ThrowAsJavaScriptException();
			return Napi::Value();
		}

		if (!info[1].IsString())
		{
			Napi::TypeError::New(env, "Second argument is expected to be a String").ThrowAsJavaScriptException();
			return Napi::Value();
		}

		if (info[2].IsEmpty())
		{
			Napi::TypeError::New(env, "Third argument is expected to be defined").ThrowAsJavaScriptException();
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Get objective function by name
	 */
	const auto objective_function = summation_objective_->GetObjectiveFunction(info[0].ToString());
	if (objective_function == nullptr)
	{
		Napi::TypeError::New(env, "Objective function could not be found").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Set property
	 */
	const std::string property_name = info[1].ToString();
	if (properties_map_.contains(property_name))
	{
		const uint32_t property_id = properties_map_.at(property_name);
		if(!objective_function->SetProperty(property_id, JSToNative(env, info[2])))
		{
			Napi::TypeError::New(env, "Couldn't set property").ThrowAsJavaScriptException();
		}

		return Napi::Value();
	}

	/**
	 * Property not found
	 */
	Napi::TypeError::New(env, "Property name could not be found").ThrowAsJavaScriptException();
	return Napi::Value();
}

Napi::Int32Array Engine::CreateBufferedFacesArray(Napi::Env env, const Eigen::MatrixXi& F) const
{
	const uint32_t entries_per_face = 3;
	auto buffered_faces_array = Napi::Int32Array::New(env, entries_per_face * F.rows());

	#pragma omp parallel for
	for (int32_t face_index = 0; face_index < F.rows(); face_index++)
	{
		const int base_index = entries_per_face * face_index;
		auto face = F.row(face_index);
		for (uint32_t i = 0; i < 3; i++)
		{
			buffered_faces_array[base_index + i] = face.coeffRef(i);
		}
	}

	return buffered_faces_array;
}

Napi::Value Engine::NativeToJS(Napi::Env env, const std::any& property_value)
{
	if (property_value.type() == typeid(double))
	{
		return NativeToJS(env, std::any_cast<const double&>(property_value));
	}

	if (property_value.type() == typeid(Eigen::VectorXd))
	{
		return NativeToJS(env, std::any_cast<const Eigen::VectorXd&>(property_value));
	}

	if (property_value.type() == typeid(std::string))
	{
		return NativeToJS(env, std::any_cast<const std::string&>(property_value));
	}

	Napi::TypeError::New(env, "Type not supported").ThrowAsJavaScriptException();
	return Napi::Value();
}

Napi::Value Engine::NativeToJS(Napi::Env env, const Eigen::VectorXd& property_value)
{
	const auto rows_count = property_value.rows();
	Napi::Array array = Napi::Array::New(env, rows_count);
	for (int32_t i = 0; i < rows_count; i++)
	{
		array[i] = Napi::Number::New(env, property_value.coeffRef(i));
	}

	return array;
}

Napi::Value Engine::NativeToJS(Napi::Env env, const double property_value)
{
	return Napi::Number::New(env, property_value);
}

Napi::Value Engine::NativeToJS(Napi::Env env, const std::string& property_value)
{
	return Napi::String::New(env, property_value);
}

std::any Engine::JSToNative(Napi::Env env, const Napi::Value& value)
{
	if (value.IsString())
	{
		return std::make_any<std::string>(value.ToString());
	}

	if (value.IsNumber())
	{
		return std::make_any<double>(value.ToNumber());
	}

	Napi::TypeError::New(env, "Type not supported").ThrowAsJavaScriptException();
	return std::any();
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

	auto objective_functions_count = summation_objective_->GetObjectiveFunctionsCount();
	Napi::Array objective_functions_data_array = Napi::Array::New(env, objective_functions_count);
	for (std::uint32_t index = 0; index < summation_objective_->GetObjectiveFunctionsCount(); index++)
	{
		Napi::Object data_object = Napi::Object::New(env);
		Napi::Object data_object_internal = Napi::Object::New(env);
		Napi::Array value_per_vertex_array = Napi::Array::New(env, mesh_wrapper_->GetImageVerticesCount());

		auto current_objective_function = summation_objective_->GetObjectiveFunction(index);
		data_object.Set("name", current_objective_function->GetName());
		data_object.Set("data", data_object_internal);
		data_object_internal.Set("value", current_objective_function->GetValue());
		data_object_internal.Set("gradientNorm", current_objective_function->GetGradient().norm());

		auto value_per_vertex = current_objective_function->GetValuePerVertex();
		for(int32_t i = 0; i < value_per_vertex.rows(); i++)
		{
			value_per_vertex_array[i] = Napi::Number::New(env, value_per_vertex.coeffRef(i));
		}
		
		//data_object_internal.Set("valuePerVertex", value_per_vertex_array);
		
		objective_functions_data_array[index] = data_object;
	}

	return objective_functions_data_array;
}

Napi::Value Engine::LoadModel(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	/**
	 * Validate input arguments
	 */
	if (info.Length() >= 1)
	{
		if (!info[0].IsString())
		{
			Napi::TypeError::New(env, "First argument is expected to be a String").ThrowAsJavaScriptException();
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
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

Napi::Value Engine::ResumeSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (newton_method_) 
	{
		newton_method_->Resume();
	}

	return env.Null();
}

Napi::Value Engine::PauseSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (newton_method_) 
	{
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
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Create a new barycenter position constraint for the given face
	 */
	int64_t face_index = info[0].As<Napi::Number>().Int64Value();
	Eigen::VectorXi indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);
	
	//std::vector<std::pair<int64_t, Eigen::Vector2d>> index_vertex_pairs;

	auto V_im = mesh_wrapper_->GetImageVertices();
	
	//for(int64_t i = 0; i < indices.rows(); i++)
	//{
	//	index_vertex_pairs.push_back({ indices.coeffRef(i,0), V_im.row(indices.coeffRef(i,0))});
	//}

	Eigen::Vector2d barycenter;
	Utils::CalculateBarycenter(indices, mesh_wrapper_->GetImageVertices(), barycenter);

	//auto vertex_position_objective = std::make_shared<VertexPositionObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, index_vertex_pairs);
	
	auto barycenter_position_objective = std::make_shared<BarycenterPositionObjective<Eigen::StorageOptions::RowMajor>>(plain_data_provider_, indices, barycenter);
	position_->AddObjectiveFunction(barycenter_position_objective);
	indices_2_position_objective_map.insert(std::make_pair(indices, barycenter_position_objective));

	return Napi::Value();
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
			return Napi::Value();
		}

		if (!info[1].IsNumber())
		{
			Napi::TypeError::New(env, "Second argument is expected to be a Number").ThrowAsJavaScriptException();
			return Napi::Value();
		}

		if (!info[2].IsNumber())
		{
			Napi::TypeError::New(env, "Third argument is expected to be a Number").ThrowAsJavaScriptException();
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
	}
 	/**
	 * Move the barycenter constraint by the given offset
	 */
	int64_t face_index = info[0].As<Napi::Number>().Int64Value();
	double offset_x = info[1].As<Napi::Number>().DoubleValue();
	double offset_y = info[2].As<Napi::Number>().DoubleValue();
	
	Eigen::Vector2d offset = Eigen::Vector2d(offset_x, offset_y);
	Eigen::VectorXi indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);

	indices_2_position_objective_map.at(indices)->OffsetPositionConstraint(offset);

	return Napi::Value();
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
			return Napi::Value();
		}
	}
	else
	{
		Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
		return Napi::Value();
	}

	/**
	 * Remove the barycenter constraint of the given face
	 */
	int64_t face_index = info[0].As<Napi::Number>().Int64Value();
	Eigen::VectorXi indices = mesh_wrapper_->GetImageFaceVerticesIndices(face_index);

	position_->RemoveObjectiveFunction(indices_2_position_objective_map.at(indices));
	indices_2_position_objective_map.erase(indices);

	return Napi::Value();
}