#ifndef ENGINE_H
#define ENGINE_H

#include <napi.h>
#include <Eigen/Core>

class Engine : public Napi::ObjectWrap<Engine> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Engine(const Napi::CallbackInfo& info);

private:
	enum class ModelFileType
	{
		OBJ,
		OFF,
		UNKNOWN
	};

	static Napi::FunctionReference constructor;

	/**
	 * NAPI Instance Methods
	 */
	Napi::Value LoadModel(const Napi::CallbackInfo& info);
	Napi::Value GetModelVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSuopVertices(const Napi::CallbackInfo& info);
	Napi::Value GetModelFaces(const Napi::CallbackInfo& info);
	Napi::Value GetSuopFaces(const Napi::CallbackInfo& info);
	Napi::Value GetModelBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSuopBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetModelBufferedMeshVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSuopBufferedMeshVertices(const Napi::CallbackInfo& info);

	/**
	 * Regular Instance Methods
	 */
	ModelFileType GetModelFileType(std::string filename);
	Napi::Array CreateVertices(Napi::Env env, Eigen::MatrixXd& V);
	Napi::Array CreateFaces(Napi::Env env, Eigen::MatrixXi& F);
	Napi::Array CreateBufferedVerticesArray(Napi::Env env, Eigen::MatrixXd& V);
	Napi::Array CreateBufferedMeshVerticesArray(Napi::Env env, Eigen::MatrixXd& V, Eigen::MatrixXi& F);

	/**
	 * Fields
	 */
	Eigen::MatrixXd V_;
	Eigen::MatrixXi F_;
};

#endif