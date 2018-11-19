#include <nan.h>
#include "anObject.h"

/*
void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New(", wangjr+Microsoft Visual Studio 2015.").ToLocalChecked());
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("hello").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Method)->GetFunction());

}


NODE_MODULE(hello, Init)
*/
void InitAll(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
{
  anObject::Init(exports);
  Nan::SetMethod(module, "exports", anObject::NewInstance);
}

NODE_MODULE(hello, InitAll)