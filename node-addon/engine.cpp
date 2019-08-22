#include "engine.h"
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

Napi::FunctionReference Engine::constructor;

Napi::Object Engine::Init(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Engine", {
	  InstanceMethod("loadModel", &Engine::LoadModel)
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