
#include "memoryobject.h"
#include "node_buffer.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> MemoryObject::constructor_template;

/* static  */
void MemoryObject::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(MemoryObject::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLMemoryObject"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getMemObjectInfo", getMemObjectInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getImageInfo", getImageInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createSubBuffer", createSubBuffer);

    target->Set(String::NewSymbol("WebCLMemoryObject"), constructor_template->GetFunction());
}

MemoryObject::MemoryObject(Handle<Object> wrapper) : mw(0)
{
    Wrap(wrapper);
}
    
MemoryObject::~MemoryObject()
{
    if (mw) mw->release();
}

/* static  */
Handle<Value> MemoryObject::getMemObjectInfo(const Arguments& args)
{
    HandleScope scope;

    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args.This());
    Local<Value> v = args[0];
    cl_mem_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];

    cl_int ret = MemoryObjectWrapper::memoryObjectInfoHelper(mo->getMemoryObjectWrapper(),
							     param_name,
							     sizeof(param_value),
							     param_value,
							     &param_value_size_ret);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_MEM_TYPE:
    case CL_MEM_FLAGS:
    case CL_MEM_REFERENCE_COUNT:
    case CL_MEM_MAP_COUNT:
	return scope.Close(Integer::NewFromUnsigned(*(cl_uint*)param_value));
    case CL_MEM_SIZE:
    case CL_MEM_OFFSET:
	return scope.Close(Number::New(*(size_t*)param_value));
    case CL_MEM_ASSOCIATED_MEMOBJECT: {
	cl_mem mem = *((cl_mem*)param_value);
	MemoryObjectWrapper *mw = new MemoryObjectWrapper(mem);
	return scope.Close(MemoryObject::New(mw)->handle_);
    }
    case CL_MEM_HOST_PTR: {
	char *ptr = *((char**)param_value);
	param_name = CL_MEM_SIZE;
	ret = MemoryObjectWrapper::memoryObjectInfoHelper(mo->getMemoryObjectWrapper(),
							  param_name,
							  sizeof(param_value),
							  param_value,
							  &param_value_size_ret);
	if (ret != CL_SUCCESS) {
	    WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}
	size_t nbytes = *(size_t*)param_value;
	return scope.Close(node::Buffer::New(ptr, nbytes)->handle_);
    }
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

/* static  */
Handle<Value> MemoryObject::getImageInfo(const Arguments& args)
{
    HandleScope scope;
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args.This());
    Local<Value> v = args[0];
    cl_mem_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];

    cl_int ret = MemoryObjectWrapper::imageInfoHelper(mo->getMemoryObjectWrapper(),
						      param_name,
						      sizeof(param_value),
						      param_value,
						      &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    switch (param_name) {
    case CL_IMAGE_ELEMENT_SIZE:
    case CL_IMAGE_ROW_PITCH:
    case CL_IMAGE_SLICE_PITCH:
    case CL_IMAGE_WIDTH:
    case CL_IMAGE_HEIGHT:
    case CL_IMAGE_DEPTH:
	return scope.Close(Number::New(*(size_t*)param_value));
    case CL_IMAGE_FORMAT: {
	cl_channel_order channel_order = *(cl_channel_order*)param_value;
	cl_channel_type channel_type = *(cl_channel_type*)(param_value + sizeof(cl_channel_order));
	Local<Object> obj = Object::New();
	obj->Set(String::New("cl_channel_order"), Number::New(channel_order));
	obj->Set(String::New("cl_channel_type"), Number::New(channel_type));
	return scope.Close(obj);
    }
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

/* static  */
Handle<Value> MemoryObject::createSubBuffer(const Arguments& args)
{
    HandleScope scope;
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args.This());

    cl_mem_flags flags = args[0]->NumberValue();

    if (flags != CL_BUFFER_CREATE_TYPE_REGION)
	return ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));

    RegionWrapper region;
    Local<Object> obj = args[1]->ToObject();
    region.origin = obj->Get(String::New("origin"))->NumberValue();
    region.size = obj->Get(String::New("size"))->NumberValue();

    MemoryObjectWrapper *mw = 0;
    cl_int ret = mo->getMemoryObjectWrapper()->createSubBuffer(flags,
							       region,
							       &mw);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_BUFFER_SIZE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_HOST_PTR);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(MemoryObject::New(mw)->handle_);
}

/* static  */
Handle<Value> MemoryObject::New(const Arguments& args)
{
    HandleScope scope;
    MemoryObject *cl = new MemoryObject(args.This());
    return args.This();
}

/* static  */
MemoryObject *MemoryObject::New(MemoryObjectWrapper* mw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    MemoryObject *memobj = ObjectWrap::Unwrap<MemoryObject>(obj);
    memobj->mw = mw;

    return memobj;
}
