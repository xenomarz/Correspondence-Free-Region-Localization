#include "../include/engine.h"
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

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
		InstanceAccessor("domainBufferedVertices", &Engine::GetDomainBufferedVertices, nullptr),
		InstanceAccessor("imageBufferedVertices", &Engine::GetImageBufferedVertices, nullptr),
		InstanceAccessor("domainBufferedMeshVertices", &Engine::GetDomainBufferedMeshVertices, nullptr),
		InstanceAccessor("imageBufferedMeshVertices", &Engine::GetImageBufferedMeshVertices, nullptr),
		InstanceAccessor("domainVertices", &Engine::GetDomainVertices, nullptr),
		InstanceAccessor("imageVertices", &Engine::GetImageVertices, nullptr),
		InstanceAccessor("domainFaces", &Engine::GetDomainFaces, nullptr),
		InstanceAccessor("imageFaces", &Engine::GetImageFaces, nullptr),
		InstanceAccessor("positionWeight", &Engine::GetPositionWeight, &Engine::SetPositionWeight),
		InstanceAccessor("seamlessWeight", &Engine::GetSeamlessWeight, &Engine::SetSeamlessWeight),
		InstanceAccessor("lambda", &Engine::GetLambda, &Engine::SetLambda),
		InstanceAccessor("delta", &Engine::GetDelta, &Engine::SetDelta)
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
	auto& x = mesh_wrapper_->GetImageVertices();
	auto x0 = Eigen::Map<const Eigen::VectorXd>(x.data(), x.cols() * x.rows());
	composite_objective_ = std::make_shared<CompositeObjective>(mesh_wrapper_);
	position_ = std::make_shared<Position>(mesh_wrapper_);
	composite_objective_->AddObjectiveFunction(position_);
	newton_method_ = std::make_unique<NewtonMethod<EigenSparseSolver>>(composite_objective_, x0);
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
	 * Get file type
	 */
	Napi::String value = info[0].As<Napi::String>();
	std::string modelFilePath = std::string(value);
	Engine::ModelFileType modelFileType = GetModelFileType(modelFilePath);
	if (modelFileType == Engine::ModelFileType::UNKNOWN)
	{
		Napi::TypeError::New(env, "Unknown file type").ThrowAsJavaScriptException();
		return env.Null();
	}

	/**
	 * Read file
	 */
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	switch (modelFileType)
	{
	case Engine::ModelFileType::OFF:
		igl::readOFF(modelFilePath, V, F);
		break;
	case Engine::ModelFileType::OBJ:
		igl::readOBJ(modelFilePath, V, F);
		break;
	}

	/**
	 * Triangulate (if faces were received as quads)
	 */
	if (F.cols() == 4)
	{
		auto F_triangulated = Eigen::MatrixXi(F.rows() * 2, 3);
		for (Eigen::DenseIndex i = 0; i < F.rows(); ++i)
		{
			auto face = F.row(i);
			auto triangle_index = 2 * i;
			F_triangulated.row(triangle_index) << face[0], face[1], face[3];
			F_triangulated.row(triangle_index + 1) << face[1], face[2], face[3];
		}

		F = F_triangulated;
	}

	/**
	 * Initialize MeshWrapper with loaded mesh model (faces and vertices)
	 */
	mesh_wrapper_ = std::make_shared<MeshWrapper>(V, F);

	return env.Null();
}

Napi::Value Engine::GetDomainVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, mesh_wrapper_->GetDomainVertices());
}

Napi::Value Engine::GetImageVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, mesh_wrapper_->GetImageVertices());
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

Napi::Value Engine::GetDomainBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, mesh_wrapper_->GetDomainVertices());
}

Napi::Value Engine::GetImageBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, mesh_wrapper_->GetImageVertices());
}

Napi::Value Engine::GetDomainBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, mesh_wrapper_->GetDomainVertices(), mesh_wrapper_->GetDomainFaces());
}

Napi::Value Engine::GetImageBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, mesh_wrapper_->GetImageVertices(), mesh_wrapper_->GetImageFaces());
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
	Napi::Array facesArray = Napi::Array::New(env);
	for (int faceIndex = 0; faceIndex < F.rows(); faceIndex++)
	{
		Napi::Object faceObject = Napi::Object::New(env);
		int v0 = F(faceIndex, 0);
		int v1 = F(faceIndex, 1);
		int v2 = F(faceIndex, 2);

		faceObject.Set("v0", v0);
		faceObject.Set("v1", v1);
		faceObject.Set("v2", v2);
		facesArray[faceIndex] = faceObject;
	}

	return facesArray;
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
	position_->SetWeight(position_weight);
}

Napi::Value Engine::GetPositionWeight(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	Napi::Number delta = Napi::Number::New(env, position_->GetWeight());

	return delta;
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
}

Napi::Value Engine::GetLambda(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return env.Null();
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
}

Napi::Value Engine::GetDelta(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return env.Null();
}

Napi::Value Engine::ResumeSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	newton_method_->Resume();

	return env.Null();
}

Napi::Value Engine::PauseSolver(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	newton_method_->Pause();

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
	for (int i = 0; i < 3; i++)
	{
		position_->AddConstrainedVertex(face_vertices_indices(i), face_vertices.row(i));
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
	for (int i = 0; i < 3; i++)
	{
		position_->OffsetConstrainedVertexPosition(face_vertices_indices(i), offset);
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
	for (int i = 0; i < 3; i++)
	{
		position_->RemoveConstrainedVertex(face_vertices_indices(i));
	}

	return env.Null();
}