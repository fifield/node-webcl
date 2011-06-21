
#include "memoryobject.h"

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
    constructor_template->SetClassName(String::NewSymbol("MemoryObject"));

    target->Set(String::NewSymbol("MemoryObject"), constructor_template->GetFunction());
}

MemoryObject::MemoryObject(Handle<Object> wrapper)
{
    Wrap(wrapper);
}
    
MemoryObject::~MemoryObject()
{
}

/* static  */
Handle<Value> MemoryObject::getMemObjectInfo(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("getMemObjectInfo unimplemented")));
}

/* static  */
Handle<Value> MemoryObject::getImageInfo(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("getImageInfo unimplemented")));
}

/* static  */
Handle<Value> MemoryObject::getSubBuffer(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("getSubBuffer unimplemented")));
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
