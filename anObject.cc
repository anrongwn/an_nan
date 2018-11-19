#include "anObject.h"
#include "async_fcb.h"

//#pragma comment(lib, "msxfs.lib")
//#pragma comment(lib, "xfs_supp.lib")

Nan::Persistent<v8::Function> anObject::constructor;
anObject::anObject(double value) : value_(value)
{
}

anObject::~anObject()
{
    WFSCleanUp();
}

void anObject::Init(v8::Local<v8::Object> exports)
{
    Nan::HandleScope scope;

    WFSVERSION xfsversion = {};
	HRESULT hr = WFSStartUp(0x00030003, &xfsversion);

    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("anObject").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "value", GetValue);
    Nan::SetPrototypeMethod(tpl, "plusOne", PlusOne);
    Nan::SetPrototypeMethod(tpl, "muliply", Multiply);
    Nan::SetPrototypeMethod(tpl, "fcb", fcb);
    Nan::SetPrototypeMethod(tpl, "async_fcb", async_fcb);

    constructor.Reset(tpl->GetFunction());

    exports->Set(Nan::New("anObject").ToLocalChecked(), tpl->GetFunction());
}

void anObject::New(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    if (info.IsConstructCall())
    {
        double value = info[0]->IsUndefined() ? 0 : info[0]->NumberValue();
        anObject *obj = new anObject(value);
        obj->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }
    else
    {
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = {info[0]};

        v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
        info.GetReturnValue().Set(cons->NewInstance(info.GetIsolate()->GetCurrentContext(),
                                                    argc, argv)
                                      .ToLocalChecked());
    }
}

void anObject::GetValue(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    anObject *obj = Nan::ObjectWrap::Unwrap<anObject>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->value_));
}

void anObject::PlusOne(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    anObject *obj = Nan::ObjectWrap::Unwrap<anObject>(info.Holder());

    //obj->value_+=1;

    info.GetReturnValue().Set(Nan::New(obj->plus()));
}

void anObject::Multiply(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    anObject *obj = ObjectWrap::Unwrap<anObject>(info.Holder());
    double multiple = info[0]->IsUndefined() ? 1 : info[0]->NumberValue();

    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);

    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {Nan::New(obj->value_ * multiple)};

    info.GetReturnValue().Set(cons->NewInstance(info.GetIsolate()->GetCurrentContext(),
                                                argc, argv)
                                  .ToLocalChecked());
}

double anObject::plus()
{
    value_ += 1;
    return value_;
}

void anObject::NewInstance(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    Nan::HandleScope scope;

    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};

    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(info.GetIsolate()->GetCurrentContext(),
                                                argc, argv)
                                  .ToLocalChecked());
}

void anObject::fcb(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    anObject *obj = Nan::ObjectWrap::Unwrap<anObject>(info.Holder());

    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(info[0]);

    if (cb->IsFunction())
    {
        const unsigned argc = 1;
        v8::Local<v8::Value> argv[argc] = {Nan::New(obj->value_)};
        cb->Call(v8::Null(isolate), argc, argv);
    }
}

void anObject::async_fcb(const Nan::FunctionCallbackInfo<v8::Value> &info){
    anObject *obj = Nan::ObjectWrap::Unwrap<anObject>(info.Holder());

    int points = To<int>(info[0]).FromJust();
    points += obj->value_;
    Callback *cb = new Callback(To<Function>(info[1]).ToLocalChecked());

    AsyncQueueWorker(new anWorker(cb, points));
}