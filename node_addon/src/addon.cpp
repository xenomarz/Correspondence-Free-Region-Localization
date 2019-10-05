#define EIGEN_USE_MKL_ALL

#include <napi.h>
#include "../include/engine.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	return Engine::Init(env, exports);
}

NODE_API_MODULE(RDSModule, InitAll)