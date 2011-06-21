
#include <iostream>

#include "common.h"
#include "platform.h"
#include "device.h"
#include "context.h"
#include "memoryobject.h"
#include "programobject.h"
#include "kernelobject.h"
#include "commandqueue.h"
#include "event.h"
#include "sampler.h"

using namespace v8;
using namespace webcl;

class WebCL : public node::ObjectWrap
{
    WebCL()
    {
    }
    
public:
    static void Init(Handle<Object> target)
    {
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->SetClassName(String::NewSymbol("WebCL"));

	NODE_SET_PROTOTYPE_METHOD(t, "getPlatformIDs", getPlatformIDs);
	NODE_SET_PROTOTYPE_METHOD(t, "createContext", createContext);
	NODE_SET_PROTOTYPE_METHOD(t, "createContextFromType", createContextFromType);
	NODE_SET_PROTOTYPE_METHOD(t, "waitForEvents", waitForEvents);

	target->Set(String::NewSymbol("WebCL"), t->GetFunction());
    }

    ~WebCL()
    {
    }
    
    static Handle<Value> New(const Arguments& args)
    {
	HandleScope scope;
	WebCL *cl = new WebCL();
	cl->Wrap(args.This());
	return args.This();
    }
    
    static Handle<Value> getPlatformIDs(const Arguments& args)
    {
	HandleScope scope;
	std::vector<PlatformWrapper*> platforms;
	cl_int ret = PlatformWrapper::getPlatforms(platforms);
	if (ret != CL_SUCCESS) {
	    WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}

	Local<Array> platformArray = Array::New(platforms.size());
	for (int i=0; i<platforms.size(); i++) {
	    platformArray->Set(i, Platform::New(platforms[i])->handle_);
	}

	return scope.Close(platformArray);
    }

    static Handle<Value> createContext(const Arguments& args)
    {
	HandleScope scope;
	if (!args[0]->IsArray())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));
	if (!args[1]->IsArray())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));

	Local<Array> propertiesArray = Array::Cast(*args[0]);
	cl_context_properties *properties = new cl_context_properties[propertiesArray->Length()+1];

	for (int i=0; i<propertiesArray->Length(); i+=2) {
	    properties[i] = (cl_context_properties)propertiesArray->Get(i)->NumberValue();
	    Local<Object> obj = propertiesArray->Get(i+1)->ToObject();
	    Platform *p = ObjectWrap::Unwrap<Platform>(obj);
	    properties[i+1] = (cl_context_properties)p->getPlatformWrapper()->getWrapped();
	}
	properties[propertiesArray->Length()] = 0;

	Local<Array> deviceArray = Array::Cast(*args[1]);
	std::vector<DeviceWrapper*> devices;
	for (int i=0; i<deviceArray->Length(); i++) {
	    Local<Object> obj = deviceArray->Get(i)->ToObject();
	    Device *d = ObjectWrap::Unwrap<Device>(obj);
	    devices.push_back(d->getDeviceWrapper());
	}

	ContextWrapper *cw = 0;
	cl_int ret = ContextWrapper::createContext (properties, devices, 0, 0, &cw);

	delete[] properties;

	if (ret != CL_SUCCESS) {
	    WEBCL_COND_RETURN_THROW(CL_INVALID_PLATFORM);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_PROPERTY);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE);
	    WEBCL_COND_RETURN_THROW(CL_DEVICE_NOT_AVAILABLE);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}

	return scope.Close(CLContext::New(cw)->handle_);
    }

    static Handle<Value> createContextFromType(const Arguments& args)
    {
	HandleScope scope;
	if (!args[0]->IsArray())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));

	Local<Array> propertiesArray = Array::Cast(*args[0]);
	std::vector<std::pair<cl_context_properties, cl_platform_id> > properties;
	for (int i=0; i<propertiesArray->Length(); i+=2) {

	    Local<Object> obj = propertiesArray->Get(i+1)->ToObject();
	    Platform *platform = ObjectWrap::Unwrap<Platform>(obj);

	    std::pair<cl_context_properties,cl_platform_id> 
		p(propertiesArray->Get(i)->NumberValue(),
		  platform->getPlatformWrapper()->getWrapped());
	    
	    properties.push_back(p);
	}

	cl_device_type device_type = args[1]->NumberValue();	

	ContextWrapper *cw = 0;
	cl_int ret = ContextWrapper::createContextFromType(properties, device_type, NULL,NULL, &cw);

	if (ret != CL_SUCCESS) {
	    WEBCL_COND_RETURN_THROW(CL_INVALID_PLATFORM);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_PROPERTY);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_DEVICE_TYPE);
	    WEBCL_COND_RETURN_THROW(CL_DEVICE_NOT_AVAILABLE);
	    WEBCL_COND_RETURN_THROW(CL_DEVICE_NOT_FOUND);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}

	return scope.Close(CLContext::New(cw)->handle_);
    }

    static Handle<Value> waitForEvents(const Arguments& args)
    {
	if (!args[0]->IsArray())
	    ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));

	Local<Array> eventsArray = Array::Cast(*args[0]);
	std::vector<const EventWrapper*> events;
	for (int i=0; i<eventsArray->Length(); i++) {
	    Local<Object> obj = eventsArray->Get(i)->ToObject();
	    EventWrapper *e = ObjectWrap::Unwrap<Event>(obj)->getEventWrapper();
	    events.push_back(e);
	}
	cl_int ret = ContextWrapper::waitForEvents(events);

	if (ret != CL_SUCCESS) {
	    WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	    WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT);
	    WEBCL_COND_RETURN_THROW(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	    WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}

	return Undefined();
    }

    static Handle<Value> unloadCompiler(const Arguments& args)
    {
	cl_int ret = ContextWrapper::unloadCompiler();

	if (ret != CL_SUCCESS) {
	    return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
	}

	return Undefined();
    }
};

//
extern "C" {
    static void init (Handle<Object> target)
    {
	WebCL::Init(target);
	Platform::Init(target);
	Device::Init(target);
	CLContext::Init(target);
	MemoryObject::Init(target);
	ProgramObject::Init(target);
	KernelObject::Init(target);
	CommandQueue::Init(target);
	Event::Init(target);
	Sampler::Init(target);
    }

    NODE_MODULE(_webcl, init);
}
