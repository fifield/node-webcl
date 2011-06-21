
#ifndef WEBCL_CONTEXT_H_
#define WEBCL_CONTEXT_H_

#include "common.h"
#include "wrapper/include/contextwrapper.h"

namespace webcl {

class CLContext : public node::ObjectWrap
{

public:
    ~CLContext();

    static void Init(v8::Handle<v8::Object> target);

    static CLContext *New(ContextWrapper* cw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getContextInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> createProgramWithSource(const v8::Arguments& args);
    static v8::Handle<v8::Value> createCommandQueue(const v8::Arguments& args);
    static v8::Handle<v8::Value> createBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> createImage2D(const v8::Arguments& args);
    static v8::Handle<v8::Value> createImage3D(const v8::Arguments& args);
    static v8::Handle<v8::Value> createSampler(const v8::Arguments& args);
    static v8::Handle<v8::Value> createUserEvent(const v8::Arguments& args);
    static v8::Handle<v8::Value> getSupportedImageFormats(const v8::Arguments& args);
    
    ContextWrapper *getContextWrapper() { return cw; };

 private:
    CLContext(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    ContextWrapper *cw;
};

} // namespace

#endif
