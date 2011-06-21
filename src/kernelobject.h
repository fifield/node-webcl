
#ifndef WEBCL_KERNELOBJECT_H_
#define WEBCL_KERNELOBJECT_H_

#include "common.h"
#include "wrapper/include/kernelwrapper.h"

namespace webcl {

class KernelObject : public node::ObjectWrap
{

public:
    ~KernelObject();

    static void Init(v8::Handle<v8::Object> target);

    static KernelObject *New(KernelWrapper* kw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getKernelInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> getKernelWorkGroupInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> setKernelArg(const v8::Arguments& args);
    
    KernelWrapper *getKernelWrapper() { return kw; };

 private:
    KernelObject(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    KernelWrapper *kw;
};

} // namespace

#endif
