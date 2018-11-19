#pragma once

#include <nan.h>

class anObject : public Nan::ObjectWrap
{
  public:
    static void Init(v8::Local<v8::Object> exports);
    static void NewInstance(const Nan::FunctionCallbackInfo<v8::Value> &info);

  private:
    explicit anObject(double value = 0);
    ~anObject();

    double value_;
    static Nan::Persistent<v8::Function> constructor;

  private:
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);

    static void GetValue(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void PlusOne(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Multiply(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void fcb(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static NAN_METHOD(async_fcb);

    double plus();
};