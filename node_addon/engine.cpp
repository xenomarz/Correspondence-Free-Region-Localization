#include "engine.h"
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

Napi::FunctionReference Engine::constructor;

Napi::Object Engine::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Engine", {
	  InstanceMethod("loadModel", &Engine::LoadModel),
	  InstanceAccessor("domainBufferedVertices", &Engine::GetDomainBufferedVertices, nullptr),
	  InstanceAccessor("imageBufferedVertices", &Engine::GetImageBufferedVertices, nullptr),
	  InstanceAccessor("domainBufferedMeshVertices", &Engine::GetDomainBufferedMeshVertices, nullptr),
	  InstanceAccessor("imageBufferedMeshVertices", &Engine::GetImageBufferedMeshVertices, nullptr),
	  InstanceAccessor("domainVertices", &Engine::GetDomainVertices, nullptr),
	  InstanceAccessor("imageVertices", &Engine::GetImageVertices, nullptr),
	  InstanceAccessor("domainFaces", &Engine::GetDomainFaces, nullptr),
	  InstanceAccessor("imageFaces", &Engine::GetImageFaces, nullptr)
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