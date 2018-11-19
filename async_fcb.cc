#include "async_fcb.h"
//#include "anObject.h"

anWorker::anWorker(Callback *cb, int point) : AsyncWorker(cb), value_(0), point_(point){

}

void anWorker::Execute()
{
    value_ += point_;
}

void anWorker::HandleOKCallback()
{
    HandleScope scope;

    Local<Value> argv[] = {
        Null(), New<Number>(value_)};

    callback->Call(2, argv, async_resource);
}

void anWorker::HandleErrorCallback()
{
    HandleScope scope;

    Local<Value> argv[] = {
        New<Number>(1),
        Null(),
    };

    callback->Call(2, argv, async_resource);
}

/*
NAN_METHOD(CalculateAsync){
    int points = To<int>(info[0]).FromJust();
    Callback *cb = new Callback(To<Function>(info[1]).ToLocalChecked());

    AsyncQueueWorker(new anWorker(cb, points));
}
*/