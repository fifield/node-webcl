#ifndef WEBCL_EVENT_H_
#define WEBCL_EVENT_H_

#include "common.h"
#include "wrapper/include/eventwrapper.h"

namespace webcl {

class Event : public node::ObjectWrap
{

public:
    ~Event();

    static void Init(v8::Handle<v8::Object> target);

    static Event *New(EventWrapper* ew);

    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getEventInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> getEventProfilingInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> setUserEventStatus(const v8::Arguments& args);

    EventWrapper *getEventWrapper() { return ew; };

 private:
    Event(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    EventWrapper *ew;
};

} // namespace

#endif
