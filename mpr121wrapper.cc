#include "mpr121wrapper.h"

#include <MPR121.h>
#include <iostream>
#include <stdlib.h>

#define NUM_ELECTRODES 12

v8::Persistent<v8::Function> MPR121Wrapper::constructor;

MPR121Wrapper::MPR121Wrapper(unsigned char address) : address_(address) {
  MPR121.begin(address);
  std::string errorStr;

  int mprError = MPR121.getError();

  // throw error if we get one
  if (mprError != NO_ERROR) {
    switch (mprError) {
      case ADDRESS_UNKNOWN:
        errorStr = "incorrect address";
        break;
      case READBACK_FAIL:
        errorStr = "readback failure";
        break;
      case OVERCURRENT_FLAG:
        errorStr = "overcurrent on REXT pin";
        break;
      case OUT_OF_RANGE:
        errorStr = "electrode out of range";
        break;
      case NOT_INITED:
        errorStr = "not initialised";
        break;
      default:
        errorStr = "unknown error";
        break;
    }

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->ThrowException(v8::String::NewFromUtf8(isolate, errorStr.c_str()));
  }
}

MPR121Wrapper::~MPR121Wrapper() {
}

void MPR121Wrapper::Init(v8::Handle<v8::Object> exports) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();

  // constructor template
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MPR121Wrapper"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "step", Step);

  NODE_SET_PROTOTYPE_METHOD(tpl, "setTouchThreshold", SetTouchThreshold);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setReleaseThreshold", SetReleaseThreshold);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setSamplePeriod", SetSamplePeriod);

  NODE_SET_PROTOTYPE_METHOD(tpl, "run", Run);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "reset", Reset);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isRunning", IsRunning);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isInited", IsInited);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "MPR121Wrapper"), tpl->GetFunction());
}

void MPR121Wrapper::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // invoked as constructor: `new MPR121Wrapper(...)`
    MPR121Wrapper *obj;

    if (args[0]->IsUndefined()) {
      // without arguments - just run
      obj = new MPR121Wrapper();
    }
    else {
      // with argument - set address:
      // convert UTF8 string from JS to std::string in C++ to unsigned int
      v8::String::Utf8Value addressUtf8(args[0]->ToString());
      std::string addressStr(*addressUtf8);
      unsigned int address = std::strtoul(addressStr.c_str(), nullptr, 16);

      obj = new MPR121Wrapper((unsigned char)address);
    }

    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  }
  else {
    // invoked as plain function `MPR121Wrapper(...)`
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void MPR121Wrapper::Step(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Handle<v8::Array> electrodes = v8::Array::New(isolate);

  MPR121.updateAll();

  // build up object for each of the electrodes:
  // { isTouched, isNewTouch, isNewRelease, filtered, baseline, touchThreshold, releaseThreshold }
  for (int i = 0; i < NUM_ELECTRODES; i++) {
    v8::Handle<v8::Object> data = v8::Object::New(isolate);

    data->Set(v8::String::NewFromUtf8(isolate, "isTouched"), v8::Boolean::New(isolate, MPR121.getTouchData(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "isNewTouch"), v8::Boolean::New(isolate, MPR121.isNewTouch(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "isNewRelease"), v8::Boolean::New(isolate, MPR121.isNewRelease(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "filtered"), v8::Number::New(isolate, MPR121.getFilteredData(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "baseline"), v8::Number::New(isolate, MPR121.getBaselineData(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "touchThreshold"), v8::Number::New(isolate, MPR121.getTouchThreshold(i)));
    data->Set(v8::String::NewFromUtf8(isolate, "releaseThreshold"), v8::Number::New(isolate, MPR121.getReleaseThreshold(i)));

    // add that object to v8 array
    electrodes->Set(i, data);
  }

  args.GetReturnValue().Set(electrodes);
}

void MPR121Wrapper::SetTouchThreshold(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.Length() == 1 && args[0]->IsNumber()) {
    // set for all electrodes
    MPR121.setTouchThreshold((int)args[0]->NumberValue());
  }
  else if (args.Length() == 2 && args[0]->IsNumber() && args[1]->IsNumber()) {
    // set for given electrode
    MPR121.setTouchThreshold((int)args[0]->NumberValue(), (int)args[1]->NumberValue());
  }
  else {
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
}

void MPR121Wrapper::SetReleaseThreshold(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.Length() == 1 && args[0]->IsNumber()) {
    // set for all electrodes
    MPR121.setReleaseThreshold((int)args[0]->NumberValue());
  }
  else if (args.Length() == 2 && args[0]->IsNumber() && args[1]->IsNumber()) {
    // set for given electrode
    MPR121.setReleaseThreshold((int)args[0]->NumberValue(), (int)args[1]->NumberValue());
  }
  else {
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
}

void MPR121Wrapper::SetSamplePeriod(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  if (args.Length() == 1 && args[0]->IsNumber()) {
    MPR121.setSamplePeriod((mpr121_sample_interval_t)args[0]->NumberValue());
  }
}

void MPR121Wrapper::Run(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  MPR121.run();

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void MPR121Wrapper::Stop(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  MPR121.stop();

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void MPR121Wrapper::Reset(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  MPR121.reset();

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void MPR121Wrapper::IsRunning(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  args.GetReturnValue().Set(v8::Boolean::New(isolate, MPR121.isRunning()));
}

void MPR121Wrapper::IsInited(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  args.GetReturnValue().Set(v8::Boolean::New(isolate, MPR121.isInited()));
}

