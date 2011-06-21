
#ifndef WEBCL_SAMPLER_H_
#define WEBCL_SAMPLER_H_

#include "common.h"
#include "wrapper/include/samplerwrapper.h"

namespace webcl {

class Sampler : public node::ObjectWrap
{

public:
    ~Sampler();

    static void Init(v8::Handle<v8::Object> target);

    static Sampler *New(SamplerWrapper* sw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getSamplerInfo(const v8::Arguments& args);
    
    SamplerWrapper *getSamplerWrapper() { return sw; };

 private:
    Sampler(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    SamplerWrapper *sw;
};

} // namespace

#endif
