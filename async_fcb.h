#pragma once;

#include <nan.h>
#include <xfsapi.h>

using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;
using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;

class anWorker : public AsyncWorker
{
  private:
    double value_;
    int point_;

    uv_async_t async;

  public:
    anWorker(Callback *cb, int point);
    void Execute();

    void HandleOKCallback();

    void HandleErrorCallback();
};
