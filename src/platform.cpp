
#include "common.h"
#include "platform.h"
#include "device.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> Platform::constructor_template;

/* static  */
void Platform::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(Platform::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLPlatform"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getPlatformInfo", getPlatformInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getDeviceIDs", getDeviceIDs);

    target->Set(String::NewSymbol("WebCLPlatform"), constructor_template->GetFunction());
}

Platform::Platform(Handle<Object> wrapper) : pw(0)
{
    Wrap(wrapper);
}
    
Platform::~Platform()
{
    if (pw) pw->release();
}

/* static */
Handle<Value> Platform::getDeviceIDs(const Arguments& args)
{
    HandleScope scope;
    std::vector<DeviceWrapper*> devices;

    Platform *platform = ObjectWrap::Unwrap<Platform>(args.This());
    Local<Value> v = args[0];
    cl_device_type device_type = v->NumberValue();

    cl_int ret = platform->getPlatformWrapper()->getDevices(device_type, devices);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PLATFORM);
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE_TYPE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_DEVICE_NOT_FOUND);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    Local<Array> deviceArray = Array::New(devices.size());
    for (int i=0; i<devices.size(); i++) {
	deviceArray->Set(i, Device::New(devices[i])->handle_);
    }

    return scope.Close(deviceArray);
}

/* static */
Handle<Value> Platform::getPlatformInfo(const Arguments& args)
{
    HandleScope scope;
    Platform *platform = ObjectWrap::Unwrap<Platform>(args.This());
    Local<Value> v = args[0];
    cl_platform_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = PlatformWrapper::platformInfoHelper(platform->getPlatformWrapper(),
						     param_name,
						     sizeof(param_value),
						     param_value,
						     &param_value_size_ret);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PLATFORM);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(String::New(param_value));
}

/* static  */
Handle<Value> Platform::New(const Arguments& args)
{
    HandleScope scope;
    Platform *cl = new Platform(args.This());
    return args.This();
}

/* static  */
Platform *Platform::New(PlatformWrapper* pw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    Platform *platform = ObjectWrap::Unwrap<Platform>(obj);
    platform->pw = pw;

    return platform;
}
