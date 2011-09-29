
#include "platform.h"
#include "commandqueue.h"
#include "context.h"
#include "device.h"
#include "memoryobject.h"
#include "event.h"
#include "kernelobject.h"

#include "node_buffer.h"

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
    constructor_template->SetClassName(String::NewSymbol("WebCLCommandQueue"));

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

    target->Set(String::NewSymbol("WebCLCommandQueue"), constructor_template->GetFunction());
}

CommandQueue::CommandQueue(Handle<Object> wrapper) : cw(0)
{
    Wrap(wrapper);
}
    
CommandQueue::~CommandQueue()
{
    if (cw) cw->release();
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
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM_EXECUTABLE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL_ARGS);
	WEBCL_COND_RETURN_THROW(CL_INVALID_WORK_DIMENSION);
	WEBCL_COND_RETURN_THROW(CL_INVALID_GLOBAL_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_INVALID_WORK_GROUP_SIZE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_WORK_ITEM_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueTask(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    // TODO: arg checking
    KernelObject *k = ObjectWrap::Unwrap<KernelObject>(args[0]->ToObject());

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[1]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueTask(k->getKernelWrapper(),
							   event_wait_list, &event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_PROGRAM_EXECUTABLE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_KERNEL_ARGS);
	WEBCL_COND_RETURN_THROW(CL_INVALID_WORK_GROUP_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueWriteBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_write = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
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
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
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
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    MemoryObject *mo_src = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    MemoryObject *mo_dst = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());

    // TODO: arg checking
    size_t src_offset = args[2]->NumberValue();
    size_t dst_offset = args[3]->NumberValue();
    size_t cb = args[4]->NumberValue();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueCopyBuffer(mo_src->getMemoryObjectWrapper(),
								 mo_dst->getMemoryObjectWrapper(),
								 src_offset,
								 dst_offset,
								 cb,
								 event_wait_list,
								 &event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_MEM_COPY_OVERLAP);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueWriteBufferRect(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_write = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    size_t buffer_origin[3];
    size_t host_origin[3];
    size_t region[3];

    Local<Array> bufferOrigin = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = bufferOrigin->Get(i)->NumberValue();
	buffer_origin[i] = s;
    }

    Local<Array> hostOrigin = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = hostOrigin->Get(i)->NumberValue();
	host_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t buffer_row_pitch = args[5]->NumberValue();
    size_t buffer_slice_pitch = args[6]->NumberValue();
    size_t host_row_pitch = args[7]->NumberValue();
    size_t host_slice_pitch = args[8]->NumberValue();

    void *ptr = args[9]->ToObject()->GetIndexedPropertiesExternalArrayData();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[10]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueWriteBufferRect(mo->getMemoryObjectWrapper(),
								      blocking_write,
								      buffer_origin,
								      host_origin,
								      region,
								      buffer_row_pitch,
								      buffer_slice_pitch,
								      host_row_pitch,
								      host_slice_pitch,
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
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueReadBufferRect(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_read = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    size_t buffer_origin[3];
    size_t host_origin[3];
    size_t region[3];

    Local<Array> bufferOrigin = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = bufferOrigin->Get(i)->NumberValue();
	buffer_origin[i] = s;
    }

    Local<Array> hostOrigin = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = hostOrigin->Get(i)->NumberValue();
	host_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t buffer_row_pitch = args[5]->NumberValue();
    size_t buffer_slice_pitch = args[6]->NumberValue();
    size_t host_row_pitch = args[7]->NumberValue();
    size_t host_slice_pitch = args[8]->NumberValue();

    void *ptr = args[9]->ToObject()->GetIndexedPropertiesExternalArrayData();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[10]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueReadBufferRect(mo->getMemoryObjectWrapper(),
								     blocking_read,
								     buffer_origin,
								     host_origin,
								     region,
								     buffer_row_pitch,
								     buffer_slice_pitch,
								     host_row_pitch,
								     host_slice_pitch,
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
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBufferRect(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo_src = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    MemoryObject *mo_dst = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());

    size_t src_origin[3];
    size_t dst_origin[3];
    size_t region[3];

    Local<Array> srcOrigin = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = srcOrigin->Get(i)->NumberValue();
	src_origin[i] = s;
    }

    Local<Array> dstOrigin = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = dstOrigin->Get(i)->NumberValue();
	dst_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t src_row_pitch = args[5]->NumberValue();
    size_t src_slice_pitch = args[6]->NumberValue();
    size_t dst_row_pitch = args[7]->NumberValue();
    size_t dst_slice_pitch = args[8]->NumberValue();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[10]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueCopyBufferRect(mo_src->getMemoryObjectWrapper(),
								     mo_dst->getMemoryObjectWrapper(),
								     src_origin,
								     dst_origin,
								     region,
								     src_row_pitch,
								     src_slice_pitch,
								     dst_row_pitch,
								     dst_slice_pitch,
								     event_wait_list,
								     &event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MEM_COPY_OVERLAP);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

     return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueWriteImage(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_write = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    size_t origin[3];
    size_t region[3];

    Local<Array> originArray = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = originArray->Get(i)->NumberValue();
	origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t row_pitch = args[4]->NumberValue();
    size_t slice_pitch = args[5]->NumberValue();

    void *ptr = args[6]->ToObject()->GetIndexedPropertiesExternalArrayData();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[7]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueWriteImage(mo->getMemoryObjectWrapper(),
								 blocking_write,
								 origin,
								 region,
								 row_pitch,
								 slice_pitch,
								 ptr,
								 event_wait_list,
								 &event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueReadImage(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    
    // TODO: arg checking
    cl_bool blocking_read = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    size_t origin[3];
    size_t region[3];

    Local<Array> originArray = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = originArray->Get(i)->NumberValue();
	origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t row_pitch = args[4]->NumberValue();
    size_t slice_pitch = args[5]->NumberValue();

    void *ptr = args[6]->ToObject()->GetIndexedPropertiesExternalArrayData();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[7]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueReadImage(mo->getMemoryObjectWrapper(),
								blocking_read,
								origin,
								region,
								row_pitch,
								slice_pitch,
								ptr,
								event_wait_list,
								&event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyImage(const Arguments& args)
{   
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo_src = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    MemoryObject *mo_dst = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());
    
    // TODO: arg checking
    size_t src_origin[3];
    size_t dst_origin[3];
    size_t region[3];

    Local<Array> srcOriginArray = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = srcOriginArray->Get(i)->NumberValue();
	src_origin[i] = s;
    }

    Local<Array> dstOriginArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = dstOriginArray->Get(i)->NumberValue();
	dst_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueCopyImage(mo_src->getMemoryObjectWrapper(),
								mo_dst->getMemoryObjectWrapper(),
								src_origin,
								dst_origin,
								region,
								event_wait_list,
								&event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_IMAGE_FORMAT_MISMATCH);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_MEM_COPY_OVERLAP);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyImageToBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo_src = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    MemoryObject *mo_dst = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());
    
    // TODO: arg checking
    size_t src_origin[3];
    size_t region[3];

    Local<Array> srcOriginArray = Array::Cast(*args[2]);
    for (int i=0; i<3; i++) {
	size_t s = srcOriginArray->Get(i)->NumberValue();
	src_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    size_t dst_offset = args[4]->NumberValue();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueCopyImageToBuffer(mo_src->getMemoryObjectWrapper(),
									mo_dst->getMemoryObjectWrapper(),
									src_origin,
									region,
									dst_offset,
									event_wait_list,
									&event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueCopyBufferToImage(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    MemoryObject *mo_src = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    MemoryObject *mo_dst = ObjectWrap::Unwrap<MemoryObject>(args[1]->ToObject());
    
    // TODO: arg checking
    size_t dst_origin[3];
    size_t region[3];

    size_t src_offset = args[2]->NumberValue();

    Local<Array> dstOriginArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = dstOriginArray->Get(i)->NumberValue();
	dst_origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueCopyBufferToImage(mo_src->getMemoryObjectWrapper(),
									mo_dst->getMemoryObjectWrapper(),
									src_offset,
									dst_origin,
									region,
									event_wait_list,
									&event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	WEBCL_COND_RETURN_THROW(CL_INVALID_IMAGE_SIZE);
	WEBCL_COND_RETURN_THROW(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_OPERATION);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);  
}

/* static */
Handle<Value> CommandQueue::enqueueMapBuffer(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    // TODO: arg checking
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    cl_bool blocking_map = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    cl_map_flags map_flags = args[2]->Uint32Value();
    size_t offset = args[3]->NumberValue();
    size_t cb = args[4]->NumberValue();

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    void *result = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueMapBuffer(mo->getMemoryObjectWrapper(),
								blocking_map,
								map_flags,
								offset,
								cb,
								event_wait_list,
								&event,
								&result);

    return scope.Close(node::Buffer::New((char*)result, cb)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueMapImage(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    // TODO: arg checking
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    cl_bool blocking_map = args[1]->BooleanValue() ? CL_TRUE : CL_FALSE;
    cl_map_flags map_flags = args[2]->Uint32Value();

    size_t origin[3];
    size_t region[3];

    Local<Array> originArray = Array::Cast(*args[3]);
    for (int i=0; i<3; i++) {
	size_t s = originArray->Get(i)->NumberValue();
	origin[i] = s;
    }

    Local<Array> regionArray = Array::Cast(*args[4]);
    for (int i=0; i<3; i++) {
	size_t s = regionArray->Get(i)->NumberValue();
	region[i] = s;
    }

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[5]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    void *result = 0;
    size_t image_row_pitch;
    size_t image_slice_pitch;

    cl_int ret = cq->getCommandQueueWrapper()->enqueueMapImage(mo->getMemoryObjectWrapper(),
							       blocking_map,
							       map_flags,
							       origin,
							       region,
							       event_wait_list,
							       &event,
							       &image_row_pitch,
							       &image_slice_pitch,
							       &result);

    // TODO: return image_row_pitch, image_slice_pitch?

    size_t nbytes = region[0] * region[1] * region[2];
    return scope.Close(node::Buffer::New((char*)result, nbytes)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueUnmapMemObject(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    // TODO: arg checking
    MemoryObject *mo = ObjectWrap::Unwrap<MemoryObject>(args[0]->ToObject());
    node::Buffer *b = ObjectWrap::Unwrap<node::Buffer>(args[1]->ToObject());
    void *mapped_ptr = node::Buffer::Data(b);

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[2]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueUnmapMemObject(mo->getMemoryObjectWrapper(),
								     mapped_ptr,
								     event_wait_list,
								     &event);
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_MEM_OBJECT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT_WAIT_LIST);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueMarker(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueMarker(&event);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return scope.Close(Event::New(event)->handle_);
}

/* static */
Handle<Value> CommandQueue::enqueueWaitForEvents(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());

    std::vector<EventWrapper*> event_wait_list;
    Local<Array> eventWaitArray = Array::Cast(*args[0]);
    for (int i=0; i<eventWaitArray->Length(); i++) {
	Local<Object> obj = eventWaitArray->Get(i)->ToObject();
	Event *e = ObjectWrap::Unwrap<Event>(obj);
	event_wait_list.push_back( e->getEventWrapper() );
    }

    EventWrapper *event = 0;
    cl_int ret = cq->getCommandQueueWrapper()->enqueueWaitForEvents(event_wait_list);

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_VALUE);
	WEBCL_COND_RETURN_THROW(CL_INVALID_CONTEXT);
	WEBCL_COND_RETURN_THROW(CL_INVALID_EVENT);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }
    
    return Undefined();
}

/* static */
Handle<Value> CommandQueue::enqueueBarrier(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cl_int ret = cq->getCommandQueueWrapper()->enqueueBarrier();

     if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return Undefined();
}

/* static */
Handle<Value> CommandQueue::finish(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cl_int ret = cq->getCommandQueueWrapper()->finish();

    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

    return Undefined();
}

/* static */
Handle<Value> CommandQueue::flush(const Arguments& args)
{
    HandleScope scope;
    CommandQueue *cq = ObjectWrap::Unwrap<CommandQueue>(args.This());
    cl_int ret = cq->getCommandQueueWrapper()->flush();
    
    if (ret != CL_SUCCESS) {
	WEBCL_COND_RETURN_THROW(CL_INVALID_COMMAND_QUEUE);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_RESOURCES);
	WEBCL_COND_RETURN_THROW(CL_OUT_OF_HOST_MEMORY);
	return ThrowException(Exception::Error(String::New("UNKNOWN ERROR")));
    }

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
