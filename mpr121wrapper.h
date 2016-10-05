#ifndef MPR121WRAPPER_H
#define MPR121WRAPPER_H

#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>

struct Work {
  uv_timer_t timer;
  v8::Persistent<v8::Function> persistentCallback;
};

class MPR121Wrapper : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);

  private:
    explicit MPR121Wrapper(unsigned char address = 0x5c);
    ~MPR121Wrapper();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    // static void timerCallback(uv_timer_t* handle);
    // static void OnCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Step(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void SetTouchThreshold(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetReleaseThreshold(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void SetSamplePeriod(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void Run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Reset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IsRunning(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IsInited(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> constructor;
    unsigned char address_;
};

#endif
