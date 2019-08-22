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

	/**
	 * Regular Instance Methods
	 */
	ModelFileType GetModelFileType(std::string filename);

	/**
	 * Fields
	 */
	Eigen::MatrixXd V_;
	Eigen::MatrixXi F_;
};

#endif