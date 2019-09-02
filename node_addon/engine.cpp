#include "engine.h"
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

Napi::FunctionReference Engine::constructor;

Napi::Object Engine::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Engine", {
	  InstanceMethod("loadModel", &Engine::LoadModel),
	  InstanceAccessor("modelBufferedVertices", &Engine::GetModelBufferedVertices, nullptr),
	  InstanceAccessor("suopBufferedVertices", &Engine::GetSuopBufferedVertices, nullptr),
	  InstanceAccessor("modelBufferedMeshVertices", &Engine::GetModelBufferedMeshVertices, nullptr),
	  InstanceAccessor("suopBufferedMeshVertices", &Engine::GetSuopBufferedMeshVertices, nullptr),
	  InstanceAccessor("modelVertices", &Engine::GetModelVertices, nullptr),
	  InstanceAccessor("suopVertices", &Engine::GetSuopVertices, nullptr),
	  InstanceAccessor("modelFaces", &Engine::GetModelFaces, nullptr),
	  InstanceAccessor("suopFaces", &Engine::GetSuopFaces, nullptr)
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Engine", func);
	return exports;
}

Engine::Engine(const Napi::CallbackInfo& info) : 
	Napi::ObjectWrap<Engine>(info)
{

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
		for (auto i = 0; i < F.rows(); ++i)
		{
			auto face = F.row(i);
			F_triangulated.row(2 * i) << face[0], face[1], face[3];
			F_triangulated.row(2 * i + 1) << face[1], face[2], face[3];
		}

		F = F_triangulated;
	}

	/**
	 * Initialize MeshWrapper with loaded mesh model (faces and vertices)
	 */
	mesh_wrapper_ = MeshWrapper(V, F);

	return env.Null();
}

Napi::Value Engine::GetModelVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, mesh_wrapper_.GetV());
}

Napi::Value Engine::GetSuopVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, mesh_wrapper_.GetVs());
}

Napi::Value Engine::GetModelFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, mesh_wrapper_.GetF());
}

Napi::Value Engine::GetSuopFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, mesh_wrapper_.GetFs());
}

Napi::Value Engine::GetModelBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, mesh_wrapper_.GetV());
}

Napi::Value Engine::GetSuopBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, mesh_wrapper_.GetVs());
}

Napi::Value Engine::GetModelBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, mesh_wrapper_.GetV(), mesh_wrapper_.GetF());
}

Napi::Value Engine::GetSuopBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, mesh_wrapper_.GetVs(), mesh_wrapper_.GetFs());
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