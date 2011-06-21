
#ifndef WEBCL_PLATFORM_H_
#define WEBCL_PLATFORM_H_

#include "common.h"
#include "wrapper/include/platformwrapper.h"

namespace webcl {

class Platform : public node::ObjectWrap
{

public:
    ~Platform();

    static void Init(v8::Handle<v8::Object> target);

    static Platform *New(PlatformWrapper* pw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getPlatformInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> getDeviceIDs(const v8::Arguments& args);
    
    PlatformWrapper *getPlatformWrapper() { return pw; };

 private:
    Platform(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    PlatformWrapper *pw;
};

} // namespace

#endif
