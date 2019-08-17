#ifndef ENGINE_H
#define ENGINE_H

#include <napi.h>

class Engine : public Napi::ObjectWrap<Engine> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Engine(const Napi::CallbackInfo& info);

private:
	static Napi::FunctionReference constructor;

	Napi::Value GetValue(const Napi::CallbackInfo& info);
	Napi::Value PlusOne(const Napi::CallbackInfo& info);
	Napi::Value Multiply(const Napi::CallbackInfo& info);

	double value_;
};

#endif