
#include "programobject.h"
#include "device.h"
#include "kernelobject.h"
#include "context.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> ProgramObject::constructor_template;

/* static  */
void ProgramObject::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(ProgramObject::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLProgram"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getProgramInfo", getProgramInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getProgramBuildInfo", getProgramBuildInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "buildProgram", buildProgram);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createKernel", createKernel);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "createKernelsInProgram", createKernelsInProgram);

    target->Set(String::NewSymbol("WebCLProgram"), constructor_template->GetFunction());
}

ProgramObject::ProgramObject(Handle<Object> wrapper) : pw(0)
{
    Wrap(wrapper);
}
    
ProgramObject::~ProgramObject()
{
    if (pw) pw->release();
}

Handle<Value> ProgramObject::getProgramInfo(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *prog = node::ObjectWrap::Unwrap<ProgramObject>(args.This());
    cl_program_info param_name = args[1]->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[4096];

    cl_int ret = ProgramWrapper::programInfoHelper(prog->getProgramWrapper(),
						   param_name,
						   sizeof(param_value),
						   param_value,
						   &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_PROGRAM_REFERENCE_COUNT:
    case CL_PROGRAM_NUM_DEVICES:
	return scope.Close(Integer::NewFromUnsigned(*(cl_uint*)param_value));
    case CL_PROGRAM_CONTEXT: {
	cl_context ctx = *((cl_context*)param_value);
	ContextWrapper *cw = new ContextWrapper(ctx);
	return scope.Close(CLContext::New(cw)->handle_);
    }
    case CL_PROGRAM_DEVICES: {
	size_t num_devices = param_value_size_ret / sizeof(cl_device_id);
	Local<Array> deviceArray = Array::New(num_devices);
	for (size_t i=0; i<num_devices; i++) {
	    cl_device_id d = ((cl_device_id*)param_value)[i];
	    DeviceWrapper *dw = new DeviceWrapper(d);
	    deviceArray->Set(i, Device::New(dw)->handle_);
	}
	return scope.Close(deviceArray);
    }
    case CL_PROGRAM_SOURCE:
	return scope.Close(String::New(param_value));
    case CL_PROGRAM_BINARY_SIZES:
	return ThrowException(Exception::Error(String::New("CL_PROGRAM_BINARY_SIZES unimplemented")));
    case CL_PROGRAM_BINARIES:
	return ThrowException(Exception::Error(String::New("CL_PROGRAM_BINARIES unimplemented")));
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

Handle<Value> ProgramObject::getProgramBuildInfo(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *prog = node::ObjectWrap::Unwrap<ProgramObject>(args.This());
    Device *dev = ObjectWrap::Unwrap<Device>(args[0]->ToObject());
    cl_program_info param_name = args[1]->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[4096];
    cl_int ret = ProgramWrapper::programBuildInfoHelper(prog->getProgramWrapper(),
							dev->getDeviceWrapper(),
							param_name,
							sizeof(param_value),
							param_value,
							&param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_PROGRAM_BUILD_STATUS:
	return scope.Close(Integer::NewFromUnsigned(*(size_t*)param_value));
    default:
	return scope.Close(String::New(param_value));
    }
}

Handle<Value> ProgramObject::buildProgram(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *prog = node::ObjectWrap::Unwrap<ProgramObject>(args.This());

    if (!args[0]->IsArray())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));
    if (!args[1]->IsString())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));

    Local<Array> deviceArray = Array::Cast(*args[0]);
    Local<Value> optionString = args[1];
    std::vector<DeviceWrapper*> devices;
    for (int i=0; i<deviceArray->Length(); i++) {
	Local<Object> obj = deviceArray->Get(i)->ToObject();
	Device *d = ObjectWrap::Unwrap<Device>(obj);
	devices.push_back( d->getDeviceWrapper() );
    }
    Handle<String> str = args[1]->ToString();
    char *c_str = new char[str->Length()+1];
    str->WriteAscii(c_str);
    std::string cpp_str(c_str);
    delete[] c_str;
    cl_int ret = prog->getProgramWrapper()->buildProgram(devices, cpp_str, 0, 0);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_BINARY);
	WEBCL_COND_RETURN_THROW(CL_INVALID_BUILD_OPTIONS);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_COMPILER_NOT_AVAILABLE);
	WEBCL_COND_RETURN_THROW(CL_BUILD_PROGRAM_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return Undefined();
}

Handle<Value> ProgramObject::createKernel(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *prog = node::ObjectWrap::Unwrap<ProgramObject>(args.This());

    Handle<String> str = args[0]->ToString();
    char *c_str = new char[str->Length()+1];
    str->WriteAscii(c_str);
    std::string cpp_str(c_str);
    delete[] c_str;

    KernelWrapper *kw = 0;

    cl_int ret = prog->getProgramWrapper()->createKernel(cpp_str, &kw);
    
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM);
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM_EXECUTABLE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL_NAME);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL_DEFINITION);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(KernelObject::New(kw)->handle_);
}

Handle<Value> ProgramObject::createKernelsInProgram(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *prog = node::ObjectWrap::Unwrap<ProgramObject>(args.This());

    std::vector<KernelWrapper*> kernels;
    cl_int ret = prog->getProgramWrapper()->createKernelsInProgram(kernels);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM);
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM_EXECUTABLE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    Local<Array> kernelArray = Array::New(kernels.size());
    for (int i=0; i<kernels.size(); i++) {
	kernelArray->Set(i, KernelObject::New(kernels[i])->handle_);
    }

    return scope.Close(kernelArray);
}

/* static  */
Handle<Value> ProgramObject::New(const Arguments& args)
{
    HandleScope scope;
    ProgramObject *cl = new ProgramObject(args.This());
    return args.This();
}

/* static  */
ProgramObject *ProgramObject::New(ProgramWrapper* pw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    ProgramObject *progobj = ObjectWrap::Unwrap<ProgramObject>(obj);
    progobj->pw = pw;

    return progobj;
}
