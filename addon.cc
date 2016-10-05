#include <node.h>
#include "mpr121wrapper.h"

void InitAll(v8::Handle<v8::Object> exports) {
  MPR121Wrapper::Init(exports);
}

NODE_MODULE(addon, InitAll)
