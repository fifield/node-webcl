
#include "platform.h"
#include "commandqueue.h"
#include "context.h"
#include "device.h"
#include "memoryobject.h"
#include "event.h"
#include "kernelobject.h"

#include <iostream>

using namespace v8;
using namespace webcl;

Persistent<FunctionTemplate> CommandQueue::constructor_template;

// e.g. IsUint32Array(v)
#define IS_BUFFER_FUNC(name, type)\
bool Is##name(v8::Handle<v8::Value> val) {\
    if (!val->IsObject()) return false;\
    v8::Local<v8::Object> obj = val->ToObject();\
    if (obj->GetIndexedPropertiesExternalArrayDataType() == type)\
	return true;\
    return false;\
}

IS_BUFFER_FUNC(Int8Array, kExternalByteArray);
IS_BUFFER_FUNC(Uint8Array, kExternalUnsignedByteArray);
IS_BUFFER_FUNC(Int16Array, kExternalShortArray);
IS_BUFFER_FUNC(Uint16Array, kExternalUnsignedShortArray);
IS_BUFFER_FUNC(Int32Array, kExternalIntArray);
IS_BUFFER_FUNC(Uint32Array, kExternalUnsignedIntArray);
IS_BUFFER_FUNC(Float32Array, kExternalFloatArray);

/* static  */
void CommandQueue::Init(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(CommandQueue::New);
    constructor_template = Persistent<FunctionTemplate>::New(t);

    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("CommandQueue"));

    NODE_SET_PROTOTYPE_METHOD(constructor_template, "getCommandQueueInfo", getCommandQueueInfo);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueNDRangeKernel", enqueueNDRangeKernel);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueTask", enqueueTask);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueWriteBuffer", enqueueWriteBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueReadBuffer", enqueueReadBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueCopyBuffer", enqueueCopyImage);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "enqueueWriteBufferRect", enqueueWriteBufferRect);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "enqueueReadBufferRect", enqueueReadBufferRect);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueCopyBufferRect", enqueueCopyImage);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueWriteImage", enqueueWriteBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueReadImage", enqueueReadBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueCopyImage", enqueueCopyImage);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "enqueueCopyImageToBuffer", enqueueCopyImageToBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, 
			      "enqueueCopyBufferToImage", enqueueCopyBufferToImage);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueMapBuffer", enqueueMapBuffer);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueMapImage", enqueueMapImage);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueUnmapMemObject", enqueueUnmapMemObject);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueMarker", enqueueMarker);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueWaitForEvents", enqueueWaitForEvents);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "enqueueBarrier", enqueueBarrier);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "flush", flush);
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "finish", finish);

    target->Set(String::NewSymbol("CommandQueue"), constructor_template->GetFunction());
}

CommandQueue::CommandQueue(Handle<Object> wrapper)
{
    Wrap(wrapper);
}
    
CommandQueue::~CommandQueue()
{
}

/* static */
Handle<Value> CommandQueue::getCommandQueueInfo(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    Local<Value> v = args[0];
    cl_command_queue_info param_name = v->NumberValue();
    size_t param_value_size_ret = 0;
    char param_value[1024];
    cl_int ret = CommandQueueWrapper::commandQueueInfoHelper(cq->getCommandQueueWrapper(),
							     param_name,
							     sizeof(param_value),
							     param_value,
							     &param_value_size_ret);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    switch (param_name) {
    case CL_QUEUE_CONTEXT: {
	cl_context ctx = *((cl_context*)param_value);
	ContextWrapper *cw = new ContextWrapper(ctx);
	return scope.Close(CLContext::New(cw)->handle_);
    }
    case CL_QUEUE_DEVICE: {
	cl_device_id dev = *((cl_device_id*)param_value);
	DeviceWrapper *dw = new DeviceWrapper(dev);
	return scope.Close(Device::New(dw)->handle_);
    }
    case CL_QUEUE_REFERENCE_COUNT:
    case CL_QUEUE_PROPERTIES:
	return scope.Close(Number::New(*(size_t*)param_value));
    default:
	return ThrowException(Exception::Error(String::New("CL_INVALID_VALUE")));
    }
}

/* static */
Handle<Value> CommandQueue::enqueueNDRangeKernel(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    // TODO: arg checking
    KernelObject *k = ObjectWrap::Unwrap<KernelObject>(args[0]->ToObject());
    cl_uint work_dim = args[1]->NumberValue();

    std::vector<size_t> global_work_offset;
    Local<Array> globalWorkOffset = Array::Cast(*args[2]);
    for (int i=0; i<globalWorkOffset->Length(); i++) {
	size_t s = globalWorkOffset->Get(i)->NumberValue();
	global_work_offset.push_back(s);
    }

    std::vector<size_t> global_work_size;
    Local<Array> globalWorkSize = Array::Cast(*args[3]);
    for (int i=0; i<globalWorkSize->Length(); i++) {
	size_t s = globalWorkSize->Get(i)->NumberValue();
	global_work_size.push_back(s);
    }

    std::vector<size_t> local_work_size;
    Local<Array> localWorkSize = Array::Cast(*args[4]);
    for (int i=0; i<localWorkSize->Length(); i++) {
	size_t s = localWorkSize->Get(i)->NumberValue();
	local_work_size.push_back(s);
    }

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueNDRangeKernel(k->getKernelWrapper(),
								    work_dim,
								    global_work_offset,
								    global_work_size,
								    local_work_size,
								    event_wait_list,
								    &event);
								    

    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueTask(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueTask unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueWriteBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_write = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;;
    size_t offset = args[2]->Uint32Value();
    size_t cb = args[3]->Uint32Value();
    void *ptr = args[4]->ToObject()->GetIndexedPropertiesExternalArrayData();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueWriteBuffer(mo->getMemoryObjectWrapper(),
								  blocking_write,
								  offset,
								  cb,
								  ptr,
								  event_wait_list,
								  &event);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueReadBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_read = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    size_t offset = args[2]->NumberValue();
    size_t cb = args[3]->NumberValue();
    void *ptr = args[4]->ToObject()->GetIndexedPropertiesExternalArrayData();
    
    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueReadBuffer(mo->getMemoryObjectWrapper(),
								 blocking_read,
								 offset,
								 cb,
								 ptr,
								 event_wait_list,
								 &event);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBuffer(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueCopyBuffer unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueWriteBufferRect(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueWriteBufferRect unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueReadBufferRect(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueReadBufferRect unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBufferRect(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueCopyBufferRect unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueWriteImage(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueWriteImage unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueReadImage(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueReadImage unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueCopyImage(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueCopyImage unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueCopyImageToBuffer(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueCopyImageToBuffer unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBufferToImage(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueCopyBufferToImage unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueMapBuffer(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueMapBuffer unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueMapImage(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueMapImage unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueUnmapMemObject(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueUnmapMemObject unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueMarker(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueMarker unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueWaitForEvents(const Arguments& args)
{
    HandleScope scope;
    return ThrowException(Exception::Error(String::New("enqueueWaitForEvents unimplemented")));
}

/* static */
Handle<Value> CommandQueue::enqueueBarrier(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cq->getCommandQueueWrapper()->enqueueBarrier();
    return Undefined();
}

/* static */
Handle<Value> CommandQueue::finish(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cq->getCommandQueueWrapper()->finish();
    return Undefined();
}

/* static */
Handle<Value> CommandQueue::flush(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cq->getCommandQueueWrapper()->flush();
    return Undefined();
}

/* static  */
Handle<Value> CommandQueue::New(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cl = new CommandQueue(args.This());
    return args.This();
}

/* static  */
CommandQueue *CommandQueue::New(CommandQueueWrapper* cw)
{

    HandleScope scope;

    Local<Value> arg = Integer::NewFromUnsigned(0);
    Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

    CommandQueue *commandqueue = ObjectWrap::Unwrap<CommandQueue>(obj);
    commandqueue->cw = cw;

    return commandqueue;
}
