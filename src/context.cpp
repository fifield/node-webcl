
#include "context.h"
#include "memoryobject.h"
#include "programobject.h"
#include "device.h"
#include "commandqueue.h"
#include "event.h"
#include "sampler.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> CLContext::constructor_template;

/* static  */
void CLContext::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(CLContext::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLContext"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getContextInfo", getContextInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "createProgramWithSource", createProgramWithSource);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createCommandQueue", createCommandQueue);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createBuffer", createBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createImage2D", createImage2D);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createImage3D", createImage3D);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createSampler", createSampler);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "createUserEvent", createUserEvent);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "getSupportedImageFormats", getSupportedImageFormats);

    target->Set(String::NewSymbol("WebCLContext"), constructor_template->GetFunction());
}

CLContext::CLContext(Handle<Object> wrapper) : cw(0)
{
    Wrap(wrapper);
}
    
CLContext::~CLContext()
{
    if (cw) cw->release();
}

/* static */
Handle<Value> CLContext::getContextInfo(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    Local<Value> v = args[0];
    cl_context_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = ContextWrapper::contextInfoHelper(context->getContextWrapper(),
						   param_name,
						   sizeof(param_value),
						   param_value,
						   &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_CONTEXT_REFERENCE_COUNT:
    case CL_CONTEXT_NUM_DEVICES:
	return scope.Close(Number::New(*(cl_uint*)param_value));
    case CL_CONTEXT_DEVICES: {
	size_t num_devices = param_value_size_ret / sizeof(cl_device_id);
	Local<Array> deviceArray = Array::New(num_devices);
	for (size_t i=0; i<num_devices; i++) {
	    cl_device_id d = ((cl_device_id*)param_value)[i];
	    DeviceWrapper *dw = new DeviceWrapper(d);
	    deviceArray->Set(i, Device::New(dw)->handle_);
	}
	return scope.Close(deviceArray); }
    case CL_CONTEXT_PROPERTIES:
	return ThrowException(Exception::Error(String::New("CL_CONTEXT_PROPERTIES unimplemented")));
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

/* static */
Handle<Value> CLContext::createProgramWithSource(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    
    ProgramWrapper *pw = 0;
    Handle<String> str = args[0]->ToString();
    char *c_str = new char[str->Length()+1];

    str->WriteAscii(c_str);
    std::string cpp_str(c_str);
    delete[] c_str;

    cl_int ret = context->getContextWrapper()->createProgramWithSource(cpp_str, &pw);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(ProgramObject::New(pw)->handle_);
}

/* static */
Handle<Value> CLContext::createCommandQueue(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = ObjectWrap::Unwrap<CLContext>(args.This());
    DeviceWrapper *device = ObjectWrap::Unwrap<Device>(args[0]->ToObject())->getDeviceWrapper();
    cl_command_queue_properties properties = args[1]->NumberValue();
    
    CommandQueueWrapper *cw = 0;
    cl_int ret = context->getContextWrapper()->createCommandQueue(device, properties, &cw);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_QUEUE_PROPERTIES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(CommandQueue::New(cw)->handle_);
}

/* static */
Handle<Value> CLContext::createBuffer(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    cl_mem_flags flags = args[0]->NumberValue();
    size_t size = args[1]->NumberValue();
    
    MemoryObjectWrapper *mw = 0;
    cl_int ret = context->getContextWrapper()->createBuffer(flags, size, 0, &mw);

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

/* static */
Handle<Value> CLContext::createImage2D(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    cl_mem_flags flags = args[0]->NumberValue();

    ImageFormatWrapper image_format;
    Local<Object> obj = args[1]->ToObject();
    image_format.channelOrder = obj->Get(String::New("cl_channel_order"))->NumberValue();
    image_format.channelDataType = obj->Get(String::New("cl_channel_type"))->NumberValue();

    size_t image_width = args[2]->NumberValue();
    size_t image_height = args[3]->NumberValue();
    size_t image_row_pitch = args[4]->NumberValue();
    
    MemoryObjectWrapper *mw = 0;
    cl_int ret = context->getContextWrapper()->createImage2D(flags,
							     image_format,
							     image_width,
							     image_height,
							     image_row_pitch,
							     0, &mw);
   if (ret != CL_SUCCESS) {
       	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_HOST_PTR);
	WEBCL_COND_RETURN_THROW(CL_IMAGE_FORMAT_NOT_SUPPORTED);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(MemoryObject::New(mw)->handle_);
}

/* static */
Handle<Value> CLContext::createImage3D(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    cl_mem_flags flags = args[0]->NumberValue();

    ImageFormatWrapper image_format;
    Local<Object> obj = args[1]->ToObject();
    image_format.channelOrder = obj->Get(String::New("cl_channel_order"))->NumberValue();
    image_format.channelDataType = obj->Get(String::New("cl_channel_type"))->NumberValue();

    size_t image_width = args[2]->NumberValue();
    size_t image_height = args[3]->NumberValue();
    size_t image_depth = args[4]->NumberValue();
    size_t image_row_pitch = args[5]->NumberValue();
    size_t image_slice_pitch = args[6]->NumberValue();
    
    MemoryObjectWrapper *mw = 0;
    cl_int ret = context->getContextWrapper()->createImage3D(flags,
							     image_format,
							     image_width,
							     image_height,
							     image_depth,
							     image_row_pitch,
							     image_slice_pitch,
							     0, &mw);
   if (ret != CL_SUCCESS) {
       	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_HOST_PTR);
	WEBCL_COND_RETURN_THROW(CL_IMAGE_FORMAT_NOT_SUPPORTED);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(MemoryObject::New(mw)->handle_);
}

/* static */
Handle<Value> CLContext::createSampler(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    cl_bool norm_coords = args[0]->BooleanValue() ? CL_TRUE : CL_FALSE;
    cl_addressing_mode addr_mode = args[1]->NumberValue();
    cl_filter_mode filter_mode = args[2]->NumberValue();

    SamplerWrapper *sw = 0;
    cl_int ret = context->getContextWrapper()->createSampler(norm_coords,
							     addr_mode,
							     filter_mode,
							     &sw);

    if (ret != CL_SUCCESS) {
       	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Sampler::New(sw)->handle_);
}

/* static */
Handle<Value> CLContext::getSupportedImageFormats(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());
    cl_mem_flags flags = args[0]->NumberValue();
    cl_mem_object_type image_type = args[1]->NumberValue();
    std::vector<ImageFormatWrapper> image_formats;

    cl_int ret = context->getContextWrapper()->getSupportedImageFormats(flags,
									image_type,
									image_formats);
    
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    Local<Array> imageFormats = Array::New();
    for (int i=0; i<image_formats.size(); i++) {
	Local<Object> format = Object::New();
	format->Set(String::New("cl_channel_order"),
		    Number::New(image_formats[i].channelOrder));
	format->Set(String::New("cl_channel_type"),
		    Number::New(image_formats[i].channelDataType));
	imageFormats->Set(i, format);
    }
       
    return scope.Close(imageFormats);
}

/* static */
Handle<Value> CLContext::createUserEvent(const Arguments& args)
{
    HandleScope scope;
    CLContext *context = node::ObjectWrap::Unwrap<CLContext>(args.This());

    EventWrapper *ew = 0;
    cl_int ret = context->getContextWrapper()->createUserEvent(&ew);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	if (ret == CL_INVALID_VALUE)
	    return ThrowException(Exception::Error(String::New("createUserEvent not enabled")));
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Event::New(ew)->handle_);
}

/* static  */
Handle<Value> CLContext::New(const Arguments& args)
{
    HandleScope scope;
    CLContext *cl = new CLContext(args.This());
    return args.This();
}

/* static  */
CLContext *CLContext::New(ContextWrapper* cw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    CLContext *context = ObjectWrap::Unwrap<CLContext>(obj);
    context->cw = cw;

    return context;
}
