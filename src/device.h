
#ifndef WEBCL_DEVICE_H_
#define WEBCL_DEVICE_H_

#include "common.h"
#include "wrapper/include/devicewrapper.h"

namespace webcl {

class Device : public node::ObjectWrap
{

public:
    ~Device();

    static void Init(v8::Handle<v8::Object> target);

    static Device *New(DeviceWrapper* dw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getDeviceInfo(const v8::Arguments& args);
    
    DeviceWrapper *getDeviceWrapper() { return dw; };

 private:
    Device(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    DeviceWrapper *dw;
};

} // namespace

#endif
