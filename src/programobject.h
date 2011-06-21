#ifndef WEBCL_PROGRAMOBJECT_H_
#define WEBCL_PROGRAMOBJECT_H_

#include "common.h"
#include "wrapper/include/programwrapper.h"

namespace webcl {

class ProgramObject : public node::ObjectWrap
{

public:
    ~ProgramObject();

    static void Init(v8::Handle<v8::Object> target);

    static ProgramObject *New(ProgramWrapper* pw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getProgramInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> getProgramBuildInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> buildProgram(const v8::Arguments& args);
    static v8::Handle<v8::Value> createKernel(const v8::Arguments& args);
    static v8::Handle<v8::Value> createKernelsInProgram(const v8::Arguments& args);

    ProgramWrapper *getProgramWrapper() { return pw; };

 private:
    ProgramObject(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    ProgramWrapper *pw;
};

} // namespace

#endif
