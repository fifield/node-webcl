
#include "event.h"

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
    constructor_template->SetClassName(String::NewSymbol("Event"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getEventInfo", getEventInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getEventProfilingInfo", getEventProfilingInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "setUserEventStatus", setUserEventStatus);
 
    target->Set(String::NewSymbol("Event"), constructor_template->GetFunction());
}

Event::Event(Handle<Object> wrapper)
{
    Wrap(wrapper);
}
    
Event::~Event()
{
}

/* static  */
Handle<Value> Event::getEventInfo(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("getEventInfo unimplemented")));
}

/* static  */
Handle<Value> Event::getEventProfilingInfo(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("getEventProfilingInfo unimplemented")));
}

/* static  */
Handle<Value> Event::setUserEventStatus(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("setUserEventStatus unimplemented")));
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

    Event *memobj = ObjectWrap::Unwrap<Event>(obj);
    memobj->ew = ew;

    return memobj;
}

