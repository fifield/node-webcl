/*
 * This file is part of WebCL – JavaScript bindings for OpenCL
 * http://webcl.nokiaresearch.com/
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Jari Nikara  ;jari.nikara@nokia.com;
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 *
 * The package is based on a published Khronos OpenCL 1.1 Specification,
 * see http://www.khronos.org/opencl/.
 *
 * OpenCL is a trademark of Apple Inc.
 */

/** \file commandqueuewrapper.cpp
 * Command queue wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "commandqueuewrapper.h"
#include "eventwrapper.h"
#include "kernelwrapper.h"
#include "memoryobjectwrapper.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

#include <cstring> //memcpy

InstanceRegistry<cl_command_queue, CommandQueueWrapper*> CommandQueueWrapper::instanceRegistry;

CommandQueueWrapper::CommandQueueWrapper (cl_command_queue aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


CommandQueueWrapper::~CommandQueueWrapper () {
    instanceRegistry.remove (mWrapped);
}


static bool unwrapEventList (std::vector<EventWrapper*> const& aWaitList,
                             cl_event** aListOut, cl_uint* aLengthOut) {
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aListOut, &err, false);
    VALIDATE_ARG_POINTER (aLengthOut, &err, false);

    cl_event* list = 0;
    cl_uint cnt = 0;

    vector<EventWrapper*>::size_type length = aWaitList.size ();
    if (length == 0)
        return true;

    list = (cl_event*)malloc (sizeof(cl_event) * length);
    if (!list) return false;

    for (vector<EventWrapper*>::size_type i = 0; i < length; ++i) {
        if (aWaitList[i]) {
            list[cnt++] = aWaitList[i]->getWrapped ();
        }
    }

    if (*aListOut) free (*aListOut);

    *aListOut = list;
    *aLengthOut = cnt;
    return true;
}


cl_int CommandQueueWrapper::enqueueNDRangeKernel (KernelWrapper* aKernel,
                                                  cl_uint aWorkDim,
                                                  std::vector<size_t> const& aGlobalWorkOffset,
                                                  std::vector<size_t> const& aGlobalWorkSize,
                                                  std::vector<size_t> const& aLocalWorkSize,
                                                  std::vector<EventWrapper*> const& aWaitList,
                                                  EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aKernel, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    size_t* globalOffset = 0;
    size_t* globalSize = 0;
    size_t* localSize = 0;

    if (aGlobalWorkOffset.size () > 0) {
        vector<size_t>::size_type length = aGlobalWorkOffset.size ();
        globalOffset = (size_t*)malloc (sizeof (size_t) * length);
        if (!globalOffset) return CL_OUT_OF_HOST_MEMORY;

        size_t* p = globalOffset;
        for (vector<size_t>::size_type i = 0; i < length; ++i, ++p)
            *p = aGlobalWorkOffset[i];
    }

    if (aGlobalWorkSize.size () > 0) {
        vector<size_t>::size_type length = aGlobalWorkSize.size ();
        globalSize = (size_t*)malloc (sizeof (size_t) * length);
        if (!globalSize) {
            if (globalOffset) free (globalOffset);
            return CL_OUT_OF_HOST_MEMORY;
        }

        size_t* p = globalSize;
        for (vector<size_t>::size_type i = 0; i < length; ++i, ++p)
            *p = aGlobalWorkSize[i];
    }

    if (aLocalWorkSize.size () > 0) {
        vector<size_t>::size_type length = aLocalWorkSize.size ();
        localSize = (size_t*)malloc (sizeof (size_t) * length);
        if (!localSize) {
            if (globalOffset) free (globalOffset);
            if (globalSize) free (globalSize);
            return CL_OUT_OF_HOST_MEMORY;
        }

        size_t* p = localSize;
        for (vector<size_t>::size_type i = 0; i < length; ++i, ++p)
            *p = aLocalWorkSize[i];
    }

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen)) {
        if (globalOffset) free (globalOffset);
        if (globalSize) free (globalSize);
        if (localSize) free (localSize);
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */
    }

    cl_event event;
    err = clEnqueueNDRangeKernel (mWrapped, aKernel->getWrapped (),
                                  aWorkDim, globalOffset,
                                  globalSize, localSize,
                                  clEvWaitListLen, clEvWaitList,
                                  &event);

    if (clEvWaitList) free (clEvWaitList);
    if (globalOffset) free (globalOffset);
    if (globalSize) free (globalSize);
    if (localSize) free (localSize);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueNDRangeKernel failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueTask (KernelWrapper* aKernel,
                                         vector<EventWrapper*> const& aWaitList,
                                         EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aKernel, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueTask (mWrapped, aKernel->getWrapped (),
                         clEvWaitListLen, clEvWaitList, &event);

    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueTask failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


#if 0
// WARNING: This function is implemented for completeness' sake and has not
//          been tested properly. For that reason it is disabled for now.
cl_int CommandQueueWrapper::enqueueNativeKernel (void (*aUserFunc)(void *),
                                                 void const* aArgs,
                                                 size_t aSizeOfArgs,
                                                 std::vector<MemoryObjectWrapper*>& aMemObjects,
                                                 std::vector<void const*> aArgsMemLoc,
                                                 std::vector<EventWrapper*> const& aWaitList,
                                                 EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_uint memObjListLen = aMemObjects.size ();
    cl_mem* memObjList = (cl_mem*)malloc (sizeof(cl_mem) * memObjListLen);
    if (memObjList) {
        vector<MemoryObjectWrapper*>::iterator i = aMemObjects.begin ();
        size_t cnt = 0;
        while (i != aMemObjects.end ()) {
            memObjList[cnt++] = (*i)->getWrapped ();
            ++i;
        }
    } else {
        if (clEvWaitList) free (clEvWaitList);
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        return CL_OUT_OF_HOST_MEMORY;
    }

    if (aArgsMemLoc.size () != aMemObjects.size ()) {
        if (clEvWaitList) free (clEvWaitList);
        if (memObjList) free (memObjList);
        D_LOG (LOG_LEVEL_ERROR,
               "The length of aArgsMemLoc (%u) does not match the length of aMemObjects (%u).",
               aArgsMemLoc.size (), aMemObjects.size ());
        return CL_INVALID_VALUE; // NOTE: synthetic err code.
    }
    void const** argsMemLocList = (void const**)malloc (sizeof(void const*) * memObjListLen);
    if (!argsMemLocList) {
        if (clEvWaitList) free (clEvWaitList);
        if (memObjList) free (memObjList);
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        return CL_OUT_OF_HOST_MEMORY;
    } else {
        vector<void const*>::iterator i = aArgsMemLoc.begin ();
        size_t cnt = 0;
        while (i != aArgsMemLoc.end ()) {
            argsMemLocList[cnt++] = *i;
            ++i;
        }
    }

    // We take a copy of the args buffer because it might be necessary to
    // modify it to replace any MemoryObjectWrapper pointers with cl_mem pointers.
    void* args = (void*)malloc (aSizeOfArgs);
    if (!args) {
        if (clEvWaitList) free (clEvWaitList);
        if (memObjList) free (memObjList);
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    memcpy (args, aArgs, aSizeOfArgs);

    // memObjListLen equals to the length of argsMemLocList
    // TODO: We should probably assume that any mem objects on aArgs are
    // actually pointers to MemoryObjectWrappers and they should be translated
    // to cl_mems here. This can be done based on aArgsMemLoc.
    for (cl_uint i = 0; i < memObjListLen; ++i) {

        // Validate the pointer: it must be within aArgs..aArgs+aSizeOfArgs.
        if (((char*)(argsMemLocList[i]) > (char*)aArgs)
            && ((char*)argsMemLocList[i] < ((char*)aArgs + aSizeOfArgs))) {
            MemoryObjectWrapper const* wrapper = *(MemoryObjectWrapper const**)(argsMemLocList[i]);
            // Translate the address to our copy of aArgs
            size_t offset = (char*)(argsMemLocList[i]) - (char*)aArgs;
            void* target = (char*)args + offset;

            *((cl_mem*)target) = wrapper->getWrapped ();
        } else {
            D_LOG (LOG_LEVEL_WARNING,
                   "Ignoring invalid argsMemLoc pointer %p at index %u: not within aArgs memory block.",
                   argsMemLocList[i], i + 1);
            // NOTE: Error message index starts from 1.
        }
    }

    cl_event event;
    err = clEnqueueNativeKernel (mWrapped, aUserFunc, args, aSizeOfArgs,
                                 memObjListLen, memObjList, argsMemLocList,
                                 clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);
    if (memObjList) free (memObjList);
    if (argsMemLocList) free (argsMemLocList);
    if (args) free (args);

    if (CL_FAILED (err)) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueNativeKernel failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}
#else
cl_int CommandQueueWrapper::enqueueNativeKernel (void (*)(void *),
                                                 void const*,
                                                 size_t,
                                                 std::vector<MemoryObjectWrapper*>&,
                                                 std::vector<void const*>,
                                                 std::vector<EventWrapper*> const&,
                                                 EventWrapper**) {
  D_LOG (LOG_LEVEL_ERROR, "NOT IMPLEMENTED!");
  return CL_INVALID_COMMAND_QUEUE;
}
#endif



cl_int CommandQueueWrapper::enqueueWriteBuffer (MemoryObjectWrapper* aBuffer,
                                                cl_bool aBlockingWrite,
                                                size_t aOffset,
                                                size_t aSize,
                                                void const* aData,
                                                vector<EventWrapper*> const& aWaitList,
                                                EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueWriteBuffer (mWrapped, aBuffer->getWrapped (),
                               aBlockingWrite, aOffset, aSize, aData,
                               clEvWaitListLen, clEvWaitList, &event);

    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueWriteBuffer failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueReadBuffer (MemoryObjectWrapper* aBuffer,
                                               cl_bool aBlockingRead,
                                               size_t aOffset,
                                               size_t aSize,
                                               void* aData,
                                               vector<EventWrapper*> const& aWaitList,
                                               EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueReadBuffer (mWrapped, aBuffer->getWrapped (),
                               aBlockingRead, aOffset, aSize, aData,
                               clEvWaitListLen, clEvWaitList, &event);

    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueReadBuffer failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueCopyBuffer (MemoryObjectWrapper* aSrcBuffer,
					       MemoryObjectWrapper* aDstBuffer,
					       const size_t aSrcOffset,
					       const size_t aDstOffset,
					       const size_t aSize,
					       std::vector<EventWrapper*> const& aWaitList,
					       EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aSrcBuffer, &err, err);
    VALIDATE_ARG_POINTER (aDstBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueCopyBuffer (mWrapped,
                               aSrcBuffer->getWrapped (), aDstBuffer->getWrapped (),
                               aSrcOffset, aDstOffset, aSize,
                               clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueCopyBuffer failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueWriteBufferRect (MemoryObjectWrapper* aBuffer,
                                                    cl_bool aBlockingWrite,
                                                    const size_t aBufferOrigin[3],
                                                    const size_t aHostOrigin[3],
                                                    const size_t aRegion[3],
                                                    size_t aBufferRowPitch,
                                                    size_t aBufferSlicePitch,
                                                    size_t aHostRowPitch,
                                                    size_t aHostSlicePitch,
                                                    void* aData,
                                                    std::vector<EventWrapper*> const& aWaitList,
                                                    EventWrapper** aResultOut) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueWriteBufferRect (mWrapped, aBuffer->getWrapped (),
                                    aBlockingWrite,
                                    aBufferOrigin, aHostOrigin, aRegion,
                                    aBufferRowPitch, aBufferSlicePitch,
                                    aHostRowPitch, aHostSlicePitch,
                                    aData,
                                    clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueWriteBufferRect failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aBuffer; (void)aBlockingWrite; (void)aBufferOrigin; (void)aHostOrigin;
    (void)aRegion; (void)aBufferRowPitch; (void)aBufferSlicePitch; (void)aHostRowPitch;
    (void)aHostSlicePitch; (void)aData; (void)aWaitList; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
}


cl_int CommandQueueWrapper::enqueueReadBufferRect (MemoryObjectWrapper* aBuffer,
                                                   cl_bool aBlockingRead,
                                                   const size_t aBufferOrigin[3],
                                                   const size_t aHostOrigin[3],
                                                   const size_t aRegion[3],
                                                   size_t aBufferRowPitch,
                                                   size_t aBufferSlicePitch,
                                                   size_t aHostRowPitch,
                                                   size_t aHostSlicePitch,
                                                   void* aData,
                                                   std::vector<EventWrapper*> const& aWaitList,
                                                   EventWrapper** aResultOut) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueReadBufferRect (mWrapped, aBuffer->getWrapped (),
                                   aBlockingRead,
                                   aBufferOrigin, aHostOrigin, aRegion,
                                   aBufferRowPitch, aBufferSlicePitch,
                                   aHostRowPitch, aHostSlicePitch,
                                   aData,
                                   clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueReadBufferRect failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aBuffer; (void)aBlockingRead; (void)aBufferOrigin; (void)aHostOrigin;
    (void)aRegion; (void)aBufferRowPitch; (void)aBufferSlicePitch; (void)aHostRowPitch;
    (void)aHostSlicePitch; (void)aData; (void)aWaitList; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
}


cl_int CommandQueueWrapper::enqueueCopyBufferRect (MemoryObjectWrapper* aSrcBuffer,
						   MemoryObjectWrapper* aDstBuffer,
                                                   const size_t aSrcOrigin[3],
                                                   const size_t aDstOrigin[3],
                                                   const size_t aRegion[3],
                                                   size_t aSrcRowPitch,
                                                   size_t aSrcSlicePitch,
                                                   size_t aDstRowPitch,
                                                   size_t aDstSlicePitch,
                                                   std::vector<EventWrapper*> const& aWaitList,
                                                   EventWrapper** aResultOut) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aSrcBuffer, &err, err);
    VALIDATE_ARG_POINTER (aDstBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueCopyBufferRect (mWrapped,
				   aSrcBuffer->getWrapped (),
				   aDstBuffer->getWrapped (),
                                   aSrcOrigin, aDstOrigin, aRegion,
                                   aSrcRowPitch, aSrcSlicePitch,
                                   aDstRowPitch, aDstSlicePitch,
                                   clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueCopyBufferRect failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aSrcBuffer; (void)aDstBuffer; (void)aSrcOrigin; (void)aDstOrigin; (void)aRegion;
    (void)aSrcRowPitch; (void)aSrcSlicePitch;  (void)aDstRowPitch; (void)aDstSlicePitch; 
    (void)aWaitList; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
}


cl_int CommandQueueWrapper::enqueueWriteImage (MemoryObjectWrapper* aImage,
                                               cl_bool aBlockingWrite,
                                               const size_t aOrigin[3],
                                               const size_t aRegion[3],
                                               size_t aInputRowPitch,
                                               size_t aInputSlicePitch,
                                               void* aData,
                                               std::vector<EventWrapper*> const& aWaitList,
                                               EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aImage, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueWriteImage (mWrapped, aImage->getWrapped (),
                               aBlockingWrite, aOrigin, aRegion,
                               aInputRowPitch, aInputSlicePitch,
                               aData,
                               clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueWriteImage failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueReadImage (MemoryObjectWrapper* aImage,
                                              cl_bool aBlockingRead,
                                              const size_t aOrigin[3],
                                              const size_t aRegion[3],
                                              size_t aRowPitch,
                                              size_t aSlicePitch,
                                              void* aData,
                                              std::vector<EventWrapper*> const& aWaitList,
                                              EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aImage, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueReadImage (mWrapped, aImage->getWrapped (),
                              aBlockingRead, aOrigin, aRegion,
                              aRowPitch, aSlicePitch, aData,
                              clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueReadImage failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueCopyImage (MemoryObjectWrapper* aSrcImage,
                                              MemoryObjectWrapper* aDstImage,
                                              const size_t aSrcOrigin[3],
                                              const size_t aDstOrigin[3],
                                              const size_t aRegion[3],
                                              std::vector<EventWrapper*> const& aWaitList,
                                              EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aSrcImage, &err, err);
    VALIDATE_ARG_POINTER (aDstImage, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueCopyImage (mWrapped,
                               aSrcImage->getWrapped (), aDstImage->getWrapped (),
                               aSrcOrigin, aDstOrigin, aRegion,
                               clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueCopyImage failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueCopyImageToBuffer (MemoryObjectWrapper* aSrcImage,
                                                      MemoryObjectWrapper* aDstBuffer,
                                                      const size_t aSrcOrigin[3],
                                                      const size_t aRegion[3],
                                                      size_t aDstOffset,
                                                      std::vector<EventWrapper*> const& aWaitList,
                                                      EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aSrcImage, &err, err);
    VALIDATE_ARG_POINTER (aDstBuffer, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueCopyImageToBuffer (mWrapped,
                                      aSrcImage->getWrapped (), aDstBuffer->getWrapped (),
                                      aSrcOrigin, aRegion, aDstOffset,
                                      clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueCopyImageToBuffer failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueCopyBufferToImage (MemoryObjectWrapper* aSrcBuffer,
                                                      MemoryObjectWrapper* aDstImage,
                                                      size_t aSrcOffset,
                                                      const size_t aDstOrigin[3],
                                                      const size_t aRegion[3],
                                                      std::vector<EventWrapper*> const& aWaitList,
                                                      EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aSrcBuffer, &err, err);
    VALIDATE_ARG_POINTER (aDstImage, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueCopyBufferToImage (mWrapped,
                                      aSrcBuffer->getWrapped (), aDstImage->getWrapped (),
                                      aSrcOffset, aDstOrigin, aRegion,
                                      clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueCopyBufferToImage failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueMapBuffer (MemoryObjectWrapper* aBuffer,
                                              cl_bool aBlockingMap,
                                              cl_map_flags aMapFlags,
                                              size_t aOffset,
                                              size_t aSize,
                                              std::vector<EventWrapper*> const& aWaitList,
                                              EventWrapper** aEventOut,
                                              void** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aBuffer, &err, err);
    VALIDATE_ARG_POINTER (aEventOut, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    *aResultOut = clEnqueueMapBuffer (mWrapped, aBuffer->getWrapped (),
                                      aBlockingMap, aMapFlags, aOffset, aSize,
                                      clEvWaitListLen, clEvWaitList, &event,
                                      &err);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueMapBuffer failed. (error %d)", err);
        return err;
    }

    *aEventOut = EventWrapper::getNewOrExisting (event);
    if (!*aEventOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueMapImage (MemoryObjectWrapper* aImage,
                                             cl_bool aBlockingMap,
                                             cl_map_flags aMapFlags,
                                             const size_t aOrigin[3],
                                             const size_t aRegion[3],
                                             std::vector<EventWrapper*> const& aWaitList,
                                             EventWrapper** aEventOut,
                                             size_t* aImageRowPitchOut,
                                             size_t* aImageSlicePitchOut,
                                             void** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aImage, &err, err);
    VALIDATE_ARG_POINTER (aEventOut, &err, err);
    VALIDATE_ARG_POINTER (aImageRowPitchOut, &err, err);
    VALIDATE_ARG_POINTER (aImageSlicePitchOut, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    *aResultOut = clEnqueueMapImage (mWrapped, aImage->getWrapped (),
                                     aBlockingMap, aMapFlags, aOrigin, aRegion,
                                     aImageRowPitchOut, aImageSlicePitchOut,
                                     clEvWaitListLen, clEvWaitList, &event,
                                     &err);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueMapImage failed. (error %d)", err);
        return err;
    }

    *aEventOut = EventWrapper::getNewOrExisting (event);
    if (!*aEventOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueUnmapMemObject (MemoryObjectWrapper* aMemObj,
                                                   void* aMappedPtr,
                                                   std::vector<EventWrapper*> const& aWaitList,
                                                   EventWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aMemObj, &err, err);
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_event event;
    err = clEnqueueUnmapMemObject (mWrapped, aMemObj->getWrapped (), aMappedPtr,
                                   clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueUnmapMemObject failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueMarker (EventWrapper** aEventOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aEventOut, &err, err);

    cl_event event;
    err = clEnqueueMarker (mWrapped, &event);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueMarker failed. (error %d)", err);
        return err;
    }

    *aEventOut = EventWrapper::getNewOrExisting (event);
    if (!*aEventOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
}


cl_int CommandQueueWrapper::enqueueWaitForEvents (std::vector<EventWrapper*> const& aWaitList) {
    D_METHOD_START;

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_int err = clEnqueueWaitForEvents (mWrapped, clEvWaitListLen, clEvWaitList);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueWaitForEvents failed. (error %d)", err);
    }

    return err;
}


cl_int CommandQueueWrapper::enqueueBarrier () {
    D_METHOD_START;
    cl_int err = clEnqueueBarrier (mWrapped);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueBarrier failed. (error %d)", err);
    }
    return err;
}


cl_int CommandQueueWrapper::flush () {
    D_METHOD_START;
    D_METHOD_START;
    cl_int err = clFlush (mWrapped);
    if (err != CL_SUCCESS)
        D_LOG (LOG_LEVEL_ERROR, "clFlush failed. (error %d)", err);
    return err;
}


cl_int CommandQueueWrapper::finish () {
    D_METHOD_START;
    cl_int err = clFinish (mWrapped);
    if (err != CL_SUCCESS)
        D_LOG (LOG_LEVEL_ERROR, "clFinish failed. (error %d)", err);
    return err;
}


cl_int CommandQueueWrapper::enqueueAcquireGLObjects (std::vector<MemoryObjectWrapper*>& aMemObjects,
                                                     std::vector<EventWrapper*> const& aWaitList,
                                                     EventWrapper** aEventOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aEventOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_uint memObjListLen = aMemObjects.size ();
    cl_mem* memObjList = (cl_mem*)malloc (sizeof(cl_mem) * memObjListLen);
    if (!memObjList) {
        if (clEvWaitList) free (clEvWaitList);
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    vector<MemoryObjectWrapper*>::iterator i = aMemObjects.begin ();
    size_t cnt = 0;
    while (i != aMemObjects.end ()) {
        memObjList[cnt++] = (*i)->getWrapped ();
        ++i;
    }

    cl_event event;
    err = clEnqueueAcquireGLObjects (mWrapped, memObjListLen, memObjList,
                                     clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);
    if (memObjList) free (memObjList);

    if (CL_FAILED (err)) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueAcquireGLObjects failed. (error %d)", err);
        return err;
    }

    *aEventOut = EventWrapper::getNewOrExisting (event);
    if (!*aEventOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aMemObjects; (void)aWaitList; (void)aEventOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


cl_int CommandQueueWrapper::enqueueReleaseGLObjects (std::vector<MemoryObjectWrapper*>& aMemObjects,
                                                     std::vector<EventWrapper*> const& aWaitList,
                                                     EventWrapper** aEventOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aEventOut, &err, err);

    cl_event* clEvWaitList = 0;
    cl_uint clEvWaitListLen = 0;
    if (!unwrapEventList (aWaitList, &clEvWaitList, &clEvWaitListLen))
        return CL_INVALID_EVENT;  /* NOTE: synthetic error code! */

    cl_uint memObjListLen = aMemObjects.size ();
    cl_mem* memObjList = (cl_mem*)malloc (sizeof(cl_mem) * memObjListLen);
    if (!memObjList) {
        if (clEvWaitList) free (clEvWaitList);
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    vector<MemoryObjectWrapper*>::iterator i = aMemObjects.begin ();
    size_t cnt = 0;
    while (i != aMemObjects.end ()) {
        memObjList[cnt++] = (*i)->getWrapped ();
        ++i;
    }

    cl_event event;
    err = clEnqueueReleaseGLObjects (mWrapped, memObjListLen, memObjList,
                                     clEvWaitListLen, clEvWaitList, &event);
    if (clEvWaitList) free (clEvWaitList);
    if (memObjList) free (memObjList);

    if (CL_FAILED (err)) {
        D_LOG (LOG_LEVEL_ERROR, "clEnqueueReleaseGLObjects failed. (error %d)", err);
        return err;
    }

    *aEventOut = EventWrapper::getNewOrExisting (event);
    if (!*aEventOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aMemObjects; (void)aWaitList; (void)aEventOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}



/* static */
CommandQueueWrapper* CommandQueueWrapper::getNewOrExisting (cl_command_queue aHandle) {
    D_METHOD_START;
    CommandQueueWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) CommandQueueWrapper (aHandle);
}


/* static */
cl_int CommandQueueWrapper::commandQueueInfoHelper (Wrapper const* aInstance, int aName,
                                                    size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    CommandQueueWrapper const* instance = dynamic_cast<CommandQueueWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetCommandQueueInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}
