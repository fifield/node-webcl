#ifndef WEBCL_MEMORYOBJECT_H_
#define WEBCL_MEMORYOBJECT_H_

#include "common.h"
#include "wrapper/include/memoryobjectwrapper.h"

namespace webcl {

class MemoryObject : public node::ObjectWrap
{

public:
    ~MemoryObject();

    static void Init(v8::Handle<v8::Object> target);

    static MemoryObject *New(MemoryObjectWrapper* mw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getMemObjectInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> getImageInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> createSubBuffer(const v8::Arguments& args);

    MemoryObjectWrapper *getMemoryObjectWrapper() { return mw; };

 private:
    MemoryObject(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    MemoryObjectWrapper *mw;
};

} // namespace

#endif
