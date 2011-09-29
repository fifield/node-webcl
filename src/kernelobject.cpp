
#include "platform.h"
#include "kernelobject.h"
#include "programobject.h"
#include "memoryobject.h"
#include "context.h"
#include "device.h"
#include "wrapper/include/clwrappertypes.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> KernelObject::constructor_template;

/* static  */
void KernelObject::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(KernelObject::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLKernel"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getKernelInfo", getKernelInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getKernelWorkgroupInfo", getKernelInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setKernelArg", setKernelArg);

    target->Set(String::NewSymbol("WebCLKernel"), constructor_template->GetFunction());
}

KernelObject::KernelObject(Handle<Object> wrapper) : kw(0)
{
    Wrap(wrapper);
}
    
KernelObject::~KernelObject()
{
    if (kw) kw->release();
}

/* static */
Handle<Value> KernelObject::getKernelInfo(const Arguments& args)
{
    HandleScope scope;
    KernelObject *kernelObject = ObjectWrap::Unwrap<KernelObject>(args.This());
    Local<Value> v = args[0];
    cl_kernel_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = KernelWrapper::kernelInfoHelper(kernelObject->getKernelWrapper(),
						 param_name,
						 sizeof(param_value),
						 param_value,
						 &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_KERNEL_FUNCTION_NAME:
	return scope.Close(String::New(param_value));
    case CL_KERNEL_NUM_ARGS:
    case CL_KERNEL_REFERENCE_COUNT:
	return scope.Close(Number::New(*(size_t*)param_value));
    case CL_KERNEL_CONTEXT: {
	cl_context ctx = *((cl_context*)param_value);
	ContextWrapper *cw = new ContextWrapper(ctx);
	return scope.Close(CLContext::New(cw)->handle_); }
    case CL_KERNEL_PROGRAM: {
	cl_program p = *((cl_program*)param_value);
	ProgramWrapper *pw = new ProgramWrapper(p);
	return scope.Close(ProgramObject::New(pw)->handle_); }
    default:
	return scope.Close(Number::New(*(size_t*)param_value));
    }
}

/* static */
Handle<Value> KernelObject::getKernelWorkGroupInfo(const Arguments& args)
{
    HandleScope scope;
    KernelObject *kernelObject = ObjectWrap::Unwrap<KernelObject>(args.This());
    Device *device = ObjectWrap::Unwrap<Device>(args[0]->ToObject());
    Local<Value> v = args[1];
    cl_kernel_work_group_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = KernelWrapper::kernelWorkGroupInfoHelper(kernelObject->getKernelWrapper(),
							  device->getDeviceWrapper(),
							  param_name,
							  sizeof(param_value),
							  param_value,
							  &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_KERNEL_WORK_GROUP_SIZE:
    case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
	return scope.Close(Number::New(*(size_t*)param_value));
    case CL_KERNEL_LOCAL_MEM_SIZE:
    case CL_KERNEL_PRIVATE_MEM_SIZE:
	return scope.Close(Number::New(*(cl_ulong*)param_value));
    case CL_KERNEL_COMPILE_WORK_GROUP_SIZE: {
	Local<Array> sizeArray = Array::New(3);
	for (size_t i=0; i<3; i++) {
	    size_t s = ((size_t*)param_value)[i];
	    sizeArray->Set(i, Number::New(s));
	}
	return scope.Close(sizeArray);
    }
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

/* static */
Handle<Value> KernelObject::setKernelArg(const Arguments& args)
{
    HandleScope scope;

   if (!args[0]->IsUint32())
       return ThrowException(Exception::Error(String::New("CL_INVALID_ARG_INDEX")));

    KernelObject *kernelObject = ObjectWrap::Unwrap<KernelObject>(args.This());
    cl_uint arg_index = args[0]->Uint32Value();
    size_t arg_size = 0;
    cl_uint type = 0;
    void *arg_value = 0;
    cl_int ret;

    type = args[2]->Uint32Value();

    switch (type) {
    case types::MEMORY_OBJECT: {
	cl_mem mem;
	if (args[1]->IsUint32()) {
	    cl_uint ptr = args[1]->Uint32Value();
	    if (ptr)
		return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	    mem = 0;
	} else {
	    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());
	    mem = mo->getMemoryObjectWrapper()->getWrapped();
	}
	arg_value = &mem;
	arg_size = sizeof(mem);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break; 
    }
    case types::UINT: {
	if (!args[1]->IsUint32())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_uint arg = args[1]->Uint32Value();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::INT: {
	if (!args[1]->IsInt32())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_uint arg = args[1]->Int32Value();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::ULONG: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_ulong arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::LONG: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_long arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::FLOAT: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_float arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::HALF: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_half arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::SHORT: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_short arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::USHORT: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_ushort arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::UCHAR: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_uchar arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::CHAR: {
	if (!args[1]->IsNumber())
	    return ThrowException(Exception::Error(String::New("ARG is not of specified type")));
	cl_char arg = args[1]->NumberValue();
	arg_value = &arg;
	arg_size = sizeof(arg);
	ret = kernelObject->getKernelWrapper()->setArg(arg_index, arg_size, arg_value);
	break;
    }
    case types::UNKNOWN:
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN TYPE")));
    }

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL);
	WEBCL_COND_RETURN_THROW(CL_INVALID_ARG_INDEX);
	WEBCL_COND_RETURN_THROW(CL_INVALID_ARG_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_SAMPLER);
	WEBCL_COND_RETURN_THROW(CL_INVALID_ARG_SIZE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return Undefined();
}

/* static  */
Handle<Value> KernelObject::New(const Arguments& args)
{
    HandleScope scope;
    KernelObject *cl = new KernelObject(args.This());
    return args.This();
}

/* static  */
KernelObject *KernelObject::New(KernelWrapper* kw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    KernelObject *kernel = ObjectWrap::Unwrap<KernelObject>(obj);
    kernel->kw = kw;

    return kernel;
}
