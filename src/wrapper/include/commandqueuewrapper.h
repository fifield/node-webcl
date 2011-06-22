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

/** \file commandqueuewrapper.h
 * Command queue wrapper class definition.
 */

#ifndef COMMANDQUEUEWRAPPER_H
#define COMMANDQUEUEWRAPPER_H

#include "clwrappercommon.h"

#include <vector>

class EventWrapper;
class KernelWrapper;
class MemoryObjectWrapper;

/** 
 *
 */
class CommandQueueWrapper : public Wrapper {
public:
    CommandQueueWrapper (cl_command_queue aHandle);
    cl_command_queue getWrapped () const { return mWrapped; }

    template<typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, commandQueueInfoHelper);
    }

    cl_int enqueueNDRangeKernel (KernelWrapper* aKernel,
                                 cl_uint aWorkDim,
                                 std::vector<size_t> const& aGlobalWorkOffset,
                                 std::vector<size_t> const& aGlobalWorkSize,
                                 std::vector<size_t> const& aLocalWorkSize,
                                 std::vector<EventWrapper*> const& aWaitList,
                                 EventWrapper** aResultOut);

    cl_int enqueueTask (KernelWrapper* aKernel,
                        std::vector<EventWrapper*> const& aWaitList,
                        EventWrapper** aResultOut);

    cl_int enqueueNativeKernel (void (*aUserFunc)(void *),
                                void const* aArgs,
                                size_t aSizeOfArgs,
                                std::vector<MemoryObjectWrapper*>& aMemObjects,
                                std::vector<void const*> aArgsMemLoc,
                                std::vector<EventWrapper*> const& aWaitList,
                                EventWrapper** aResultOut);

    cl_int enqueueWriteBuffer (MemoryObjectWrapper* aBuffer,
                               cl_bool aBlockingWrite,
                               size_t aOffset,
                               size_t aSize,
                               void const* aData,
                               std::vector<EventWrapper*> const& aWaitList,
                               EventWrapper** aResultOut);

    cl_int enqueueReadBuffer (MemoryObjectWrapper* aBuffer,
                              cl_bool aBlockingRead,
                              size_t aOffset,
                              size_t aSize,
                              void* aData,
                              std::vector<EventWrapper*> const& aWaitList,
                              EventWrapper** aResultOut);

    cl_int enqueueCopyBuffer (MemoryObjectWrapper* aSrcBuffer,
			      MemoryObjectWrapper* aDstBuffer,
			      const size_t aSrcOffset,
			      const size_t aDstOffset,
			      const size_t aSize,
			      std::vector<EventWrapper*> const& aWaitList,
			      EventWrapper** aResultOut);

    // Note: OpenCL 1.1
    cl_int enqueueWriteBufferRect (MemoryObjectWrapper* aBuffer,
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
                                   EventWrapper** aResultOut);

    // Note: OpenCL 1.1
    cl_int enqueueReadBufferRect (MemoryObjectWrapper* aBuffer,
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
                                  EventWrapper** aResultOut);

    // Note: OpenCL 1.1
    cl_int enqueueCopyBufferRect (MemoryObjectWrapper* aSrcBuffer,
				  MemoryObjectWrapper* aDstBuffer,
				  const size_t aSrcOrigin[3],
				  const size_t aDstOrigin[3],
				  const size_t aRegion[3],
				  size_t aSrcRowPitch,
				  size_t aSrcSlicePitch,
				  size_t aDstRowPitch,
				  size_t aDstSlicePitch,
                                  std::vector<EventWrapper*> const& aWaitList,
                                  EventWrapper** aResultOut);

    cl_int enqueueWriteImage (MemoryObjectWrapper* aImage,
                              cl_bool aBlockingWrite,
                              const size_t aOrigin[3],
                              const size_t aRegion[3],
                              size_t aInputRowPitch,
                              size_t aInputSlicePitch,
                              void* aData,
                              std::vector<EventWrapper*> const& aWaitList,
                              EventWrapper** aResultOut);

    cl_int enqueueReadImage (MemoryObjectWrapper* aImage,
                             cl_bool aBlockingRead,
                             const size_t aOrigin[3],
                             const size_t aRegion[3],
                             size_t aRowPitch,
                             size_t aSlicePitch,
                             void* aData,
                             std::vector<EventWrapper*> const& aWaitList,
                             EventWrapper** aResultOut);

    cl_int enqueueCopyImage (MemoryObjectWrapper* aSrcImage,
                             MemoryObjectWrapper* aDstImage,
                             const size_t aSrcOrigin[3],
                             const size_t aDstOrigin[3],
                             const size_t aRegion[3],
                             std::vector<EventWrapper*> const& aWaitList,
                             EventWrapper** aResultOut);

    cl_int enqueueCopyImageToBuffer (MemoryObjectWrapper* aSrcImage,
                                     MemoryObjectWrapper* aDstBuffer,
                                     const size_t aSrcOrigin[3],
                                     const size_t aRegion[3],
                                     size_t aDstOffset,
                                     std::vector<EventWrapper*> const& aWaitList,
                                     EventWrapper** aResultOut);

    cl_int enqueueCopyBufferToImage (MemoryObjectWrapper* aSrcBuffer,
                                     MemoryObjectWrapper* aDstImage,
                                     size_t aSrcOffset,
                                     const size_t aDstOrigin[3],
                                     const size_t aRegion[3],
                                     std::vector<EventWrapper*> const& aWaitList,
                                     EventWrapper** aResultOut);

    cl_int enqueueMapBuffer (MemoryObjectWrapper* aBuffer,
                             cl_bool aBlockingMap,
                             cl_map_flags aMapFlags,
                             size_t aOffset,
                             size_t aSize,
                             std::vector<EventWrapper*> const& aWaitList,
                             EventWrapper** aEventOut,
                             void** aResultOut);

    cl_int enqueueMapImage (MemoryObjectWrapper* aImage,
                            cl_bool aBlockingMap,
                            cl_map_flags aMapFlags,
                            const size_t aOrigin[3],
                            const size_t aRegion[3],
                            std::vector<EventWrapper*> const& aWaitList,
                            EventWrapper** aEventOut,
                            size_t* aImageRowPitchOut,
                            size_t* aImageSlicePitchOut,
                            void** aResultOut);

    cl_int enqueueUnmapMemObject (MemoryObjectWrapper* aMemObj,
                                  void* aMappedPtr,
                                  std::vector<EventWrapper*> const& aWaitList,
                                  EventWrapper** aResultOut);

    cl_int enqueueMarker (EventWrapper** aEventOut);

    cl_int enqueueWaitForEvents (std::vector<EventWrapper*> const& aWaitList);

    cl_int enqueueBarrier ();

    cl_int flush ();

    cl_int finish ();

    // Note: OpenCL/OpenGL interop 1.0
    cl_int enqueueAcquireGLObjects (std::vector<MemoryObjectWrapper*>& aMemObjects,
                                    std::vector<EventWrapper*> const& aWaitList,
                                    EventWrapper** aEventOut);

    // Note: OpenCL/OpenGL interop 1.0
    cl_int enqueueReleaseGLObjects (std::vector<MemoryObjectWrapper*>& aMemObjects,
                                    std::vector<EventWrapper*> const& aWaitList,
                                    EventWrapper** aEventOut);

protected:
    virtual ~CommandQueueWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainCommandQueue (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseCommandQueue (mWrapped); }

private:
    CommandQueueWrapper ();
    cl_command_queue mWrapped;

public:
    static InstanceRegistry<cl_command_queue, CommandQueueWrapper*> instanceRegistry;
    static CommandQueueWrapper* getNewOrExisting (cl_command_queue aHandle);

    static cl_int commandQueueInfoHelper (Wrapper const* aInstance, int aName,
                                          size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // COMMANDQUEUEWRAPPER_H
