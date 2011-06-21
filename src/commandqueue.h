
#ifndef WEBCL_COMMANDQUEUE_H_
#define WEBCL_COMMANDQUEUE_H_

#include "common.h"
#include "wrapper/include/commandqueuewrapper.h"

namespace webcl {

class CommandQueue : public node::ObjectWrap
{

public:
    ~CommandQueue();

    static void Init(v8::Handle<v8::Object> target);

    static CommandQueue *New(CommandQueueWrapper* cw);
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

    static v8::Handle<v8::Value> getCommandQueueInfo(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueNDRangeKernel(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueTask(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueWriteBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueReadBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueCopyBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueWriteBufferRect(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueReadBufferRect(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueCopyBufferRect(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueWriteImage(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueReadImage(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueCopyImage(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueCopyImageToBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueCopyBufferToImage(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueMapBuffer(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueMapImage(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueUnmapMemObject(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueMarker(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueWaitForEvents(const v8::Arguments& args);
    static v8::Handle<v8::Value> enqueueBarrier(const v8::Arguments& args);
    static v8::Handle<v8::Value> flush(const v8::Arguments& args);
    static v8::Handle<v8::Value> finish(const v8::Arguments& args);
    
    CommandQueueWrapper *getCommandQueueWrapper() { return cw; };

 private:
    CommandQueue(v8::Handle<v8::Object> wrapper);

    static v8::Persistent<v8::FunctionTemplate> constructor_template;

    CommandQueueWrapper *cw;
};

} // namespace

#endif
