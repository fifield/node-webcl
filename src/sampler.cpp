
#include "platform.h"
#include "sampler.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> Sampler::constructor_template;

/* static  */
void Sampler::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(Sampler::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("Sampler"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getSamplerInfo", getSamplerInfo);

    target->Set(String::NewSymbol("Sampler"), constructor_template->GetFunction());
}

Sampler::Sampler(Handle<Object> wrapper)
{
    Wrap(wrapper);
}
    
Sampler::~Sampler()
{
}

/* static */
Handle<Value> Sampler::getSamplerInfo(const Arguments& args)
{
    HandleScope scope;
    Sampler *sampler = ObjectWrap::Unwrap<Sampler>(args.This());
    Local<Value> v = args[0];
    cl_sampler_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];

    return ThrowException(Exception::Error(String::New("getSamplerInfo unimplemented")));
    
    cl_int ret = SamplerWrapper::samplerInfoHelper(sampler->getSamplerWrapper(),
						   param_name,
						   sizeof(param_value),
						   param_value,
						   &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    }

}

/* static  */
Handle<Value> Sampler::New(const Arguments& args)
{
    HandleScope scope;
    Sampler *cl = new Sampler(args.This());
    return args.This();
}

/* static  */
Sampler *Sampler::New(SamplerWrapper* sw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    Sampler *sampler = ObjectWrap::Unwrap<Sampler>(obj);
    sampler->sw = sw;

    return sampler;
}
