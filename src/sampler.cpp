
#include "platform.h"
#include "sampler.h"
#include "context.h"

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
    constructor_template->SetClassName(String::NewSymbol("WebCLSampler"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getSamplerInfo", getSamplerInfo);

    target->Set(String::NewSymbol("WebCLSampler"), constructor_template->GetFunction());
}

Sampler::Sampler(Handle<Object> wrapper) : sw(0)
{
    Wrap(wrapper);
}
    
Sampler::~Sampler()
{
    if (sw) sw->release();
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

    cl_int ret = SamplerWrapper::samplerInfoHelper(sampler->getSamplerWrapper(),
						   param_name,
						   sizeof(param_value),
						   param_value,
						   &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_SAMPLER);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_SAMPLER_ADDRESSING_MODE:
    case CL_SAMPLER_FILTER_MODE:
    case CL_SAMPLER_REFERENCE_COUNT:
	return scope.Close(Number::New(*(cl_uint*)param_value));
    case CL_SAMPLER_CONTEXT:{
	cl_context ctx = *((cl_context*)param_value);
	ContextWrapper *cw = new ContextWrapper(ctx);
	return scope.Close(CLContext::New(cw)->handle_);
    }
    case CL_SAMPLER_NORMALIZED_COORDS: {
	cl_bool b = *((cl_bool*)param_value);
	return scope.Close(Boolean::New(b ? true : false ));
    }
    default:
	return ThrowException(Exception::Error(String::New("UNKNOWN param_name")));
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
