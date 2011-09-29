
#include "event.h"
#include "context.h"
#include "commandqueue.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> Event::constructor_template;

/* static  */
void Event::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(Event::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("WebCLEvent"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getEventInfo", getEventInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getEventProfilingInfo", getEventProfilingInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setUserEventStatus", setUserEventStatus);
 
    target->Set(String::NewSymbol("WebCLEvent"), constructor_template->GetFunction());
}

Event::Event(Handle<Object> wrapper) : ew(0)
{
    Wrap(wrapper);
}
    
Event::~Event()
{
    if (ew) ew->release();
}

/* static  */
Handle<Value> Event::getEventInfo(const Arguments& args)
{
    HandleScope scope;
    Event *e = ObjectWrap::Unwrap<Event>(args.This());
    cl_event_info param_name = args[0]->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];

    cl_int ret = EventWrapper::eventInfoHelper(e->getEventWrapper(),
					       param_name,
					       sizeof(param_value),
					       param_value,
					       &param_value_size_ret);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_EVENT_CONTEXT:{
	cl_context ctx = *((cl_context*)param_value);
	ContextWrapper *cw = new ContextWrapper(ctx);
	return scope.Close(CLContext::New(cw)->handle_);
    }
    case CL_EVENT_COMMAND_QUEUE:{
	cl_command_queue q = *((cl_command_queue*)param_value);
	CommandQueueWrapper *qw = new CommandQueueWrapper(q);
	return scope.Close(CommandQueue::New(qw)->handle_);
    }
    case CL_EVENT_REFERENCE_COUNT:
	return scope.Close(Number::New(*(cl_uint*)param_value));
    case CL_EVENT_COMMAND_TYPE:
    case CL_EVENT_COMMAND_EXECUTION_STATUS:
	return scope.Close(Number::New(*(cl_int*)param_value));
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
 
}

/* static  */
Handle<Value> Event::getEventProfilingInfo(const Arguments& args)
{
    HandleScope scope;
    Event *e = ObjectWrap::Unwrap<Event>(args.This());
    cl_event_info param_name = args[0]->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];

    cl_int ret = EventWrapper::eventProfilingInfoHelper(e->getEventWrapper(),
							param_name,
							sizeof(param_value),
							param_value,
							&param_value_size_ret);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_PROFILING_INFO_NOT_AVAILABLE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_PROFILING_COMMAND_QUEUED:
    case CL_PROFILING_COMMAND_SUBMIT:
    case CL_PROFILING_COMMAND_START:
    case CL_PROFILING_COMMAND_END:
	return scope.Close(Number::New(*(cl_ulong*)param_value));
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
    }
}

/* static  */
Handle<Value> Event::setUserEventStatus(const Arguments& args)
{
    HandleScope scope;
    Event *e = ObjectWrap::Unwrap<Event>(args.This());

    cl_int ret = e->getEventWrapper()->setUserEventStatus((cl_int)args[0]->NumberValue());
    
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return Undefined();
}

/* static  */
Handle<Value> Event::New(const Arguments& args)
{
    HandleScope scope;
    Event *cl = new Event(args.This());
    return args.This();
}

/* static  */
Event *Event::New(EventWrapper* ew)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    Event *e = ObjectWrap::Unwrap<Event>(obj);
    e->ew = ew;

    return e;
}

