#include <nan.h>
#include "AutocutsEngine.h"

NAN_MODULE_INIT(InitAll)
{
  AutocutsEngine::Init(target);
}

NODE_MODULE(AutocutsModule, InitAll)