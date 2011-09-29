
#include "platform.h"
#include "device.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> Device::constructor_template;

/* static  */
void Device::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(Device::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLDevice"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getDeviceInfo", getDeviceInfo);

    target->Set(String::NewSymbol("WebCLDevice"), constructor_template->GetFunction());
}

Device::Device(Handle<Object> wrapper) : dw(0)
{
    Wrap(wrapper);
}
    
Device::~Device()
{
    if (dw) dw->release();
}

/* static */
Handle<Value> Device::getDeviceInfo(const Arguments& args)
{
    HandleScope scope;
    Device *device = ObjectWrap::Unwrap<Device>(args.This());
    Local<Value> v = args[0];
    cl_device_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = DeviceWrapper::deviceInfoHelper(device->getDeviceWrapper(),
						 param_name,
						 sizeof(param_value),
						 param_value,
						 &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_DEVICE_NAME:
    case CL_DEVICE_VENDOR:
    case CL_DRIVER_VERSION:
    case CL_DEVICE_PROFILE:
    case CL_DEVICE_VERSION:
    case CL_DEVICE_OPENCL_C_VERSION:
    case CL_DEVICE_EXTENSIONS:
	return scope.Close(String::New(param_value));
    default:
	return scope.Close(Number::New(*(size_t*)param_value));
    }
}

/* static  */
Handle<Value> Device::New(const Arguments& args)
{
    HandleScope scope;
    Device *cl = new Device(args.This());
    return args.This();
}

/* static  */
Device *Device::New(DeviceWrapper* dw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    Device *device = ObjectWrap::Unwrap<Device>(obj);
    device->dw = dw;

    return device;
}
