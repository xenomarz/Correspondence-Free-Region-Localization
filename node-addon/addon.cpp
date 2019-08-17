#include <napi.h>
#include "engine.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	return Engine::Init(env, exports);
}

NODE_API_MODULE(RDSModule, InitAll)