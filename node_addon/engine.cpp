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

Engine::Engine(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Engine>(info)
{

}

Napi::Value Engine::LoadModel(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);
	if (info.Length() <= 0 || !info[0].IsString()) 
	{
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
		return env.Null();
	}

	Napi::String value = info[0].As<Napi::String>();
	std::string modelFilePath = std::string(value);
	Engine::ModelFileType modelFileType = GetModelFileType(modelFilePath);
	if (modelFileType == Engine::ModelFileType::UNKNOWN)
	{
		Napi::TypeError::New(env, "Unknown file type").ThrowAsJavaScriptException();
		return env.Null();
	}

	switch (modelFileType)
	{
	case Engine::ModelFileType::OFF:
		igl::readOFF(modelFilePath, V_, F_);
		break;
	case Engine::ModelFileType::OBJ:
		igl::readOBJ(modelFilePath, V_, F_);
		break;
	}

	return env.Null();
}

Napi::Value Engine::GetModelVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, V_);
}

Napi::Value Engine::GetSuopVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateVertices(env, V_);
}

Napi::Value Engine::GetModelFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, F_);
}

Napi::Value Engine::GetSuopFaces(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateFaces(env, F_);
}

Napi::Value Engine::GetModelBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, V_);
}

Napi::Value Engine::GetSuopBufferedVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedVerticesArray(env, V_);
}

Napi::Value Engine::GetModelBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, V_, F_);
}

Napi::Value Engine::GetSuopBufferedMeshVertices(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	return CreateBufferedMeshVerticesArray(env, V_, F_);
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

Napi::Array Engine::CreateVertices(Napi::Env env, Eigen::MatrixXd& V)
{
	Napi::Array verticesArray = Napi::Array::New(env);
	for (int vertexIndex = 0; vertexIndex < V.rows(); vertexIndex++)
	{
		Napi::Object vertexObject = Napi::Object::New(env);
		float x = V(vertexIndex, 0);
		float y = V(vertexIndex, 1);
		float z = 0;

		vertexObject.Set("x", x);
		vertexObject.Set("y", y);
		vertexObject.Set("z", z);
		verticesArray[vertexIndex] = vertexObject;
	}

	return verticesArray;
}

Napi::Array Engine::CreateFaces(Napi::Env env, Eigen::MatrixXi& F)
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

Napi::Array Engine::CreateBufferedVerticesArray(Napi::Env env, Eigen::MatrixXd& V)
{
	Napi::Array bufferedVerticesArray = Napi::Array::New(env);
	for (int vertexIndex = 0; vertexIndex < V.rows(); vertexIndex++)
	{
		float x = V(vertexIndex, 0);
		float y = V(vertexIndex, 1);
		float z = V(vertexIndex, 2);

		int baseIndex = 3 * vertexIndex;
		bufferedVerticesArray[baseIndex] = x;
		bufferedVerticesArray[baseIndex + 1] = y;
		bufferedVerticesArray[baseIndex + 2] = z;
	}

	return bufferedVerticesArray;
}

Napi::Array Engine::CreateBufferedMeshVerticesArray(Napi::Env env, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
	Napi::Array bufferedMeshVerticesArray = Napi::Array::New(env);
	for (int faceIndex = 0; faceIndex < F.rows(); faceIndex++)
	{
		int baseIndex = 9 * faceIndex;
		for (int i = 0; i < 3; i++)
		{
			int vertexIndex = F(faceIndex, i);
			float x = V(vertexIndex, 0);
			float y = V(vertexIndex, 1);
			float z = V(vertexIndex, 2);

			int baseVertexIndex = baseIndex + 3 * i;
			bufferedMeshVerticesArray[baseVertexIndex] = x;
			bufferedMeshVerticesArray[baseVertexIndex + 1] = y;
			bufferedMeshVerticesArray[baseVertexIndex + 2] = z;
		}
	}

	return bufferedMeshVerticesArray;
}
