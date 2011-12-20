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

/** \file contextwrapper.h
 * Context wrapper class definition.
 */

#ifndef CONTEXTWRAPPER_H
#define CONTEXTWRAPPER_H

#include "clwrappercommon.h"

#include <string>
#include <vector>

class PlatformWrapper;
class DeviceWrapper;
class EventWrapper;
class ProgramWrapper;
class CommandQueueWrapper;
class MemoryObjectWrapper;
class SamplerWrapper;


class ContextWrapper : public Wrapper {
public:
    ContextWrapper (cl_context aHandle);
    cl_context getWrapped () const { return mWrapped; }

    template<typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, contextInfoHelper);
    }

    cl_int createProgramWithSource (std::string const& aSource,
                                    ProgramWrapper** aProgramOut);

    cl_int createProgramWithBinary (std::vector<DeviceWrapper*> const& aDevices,
                                    std::vector<std::string const*> const& aBinaries,
                                    std::vector<cl_int>& aBinaryStatusOut,
                                    ProgramWrapper** aProgramOut);

    cl_int createCommandQueue (DeviceWrapper* aDevice,
                               cl_command_queue_properties aProperties,
                               CommandQueueWrapper** aCmdQueueOut);

    cl_int createBuffer (cl_mem_flags aFlags, size_t aSize, void* aHostPtr,
                         MemoryObjectWrapper** aResultOut);

    cl_int createImage2D (cl_mem_flags aFlags,
                          ImageFormatWrapper const& aImageFormat,
                          size_t aWidth, size_t aHeight, size_t aRowPitch,
                          void* aHostPtr, MemoryObjectWrapper** aResultOut);

    cl_int createImage3D (cl_mem_flags aFlags,
                          ImageFormatWrapper const& aImageFormat,
                          size_t aWidth, size_t aHeight, size_t aDepth,
                          size_t aRowPitch, size_t aSlicePitch,
                          void* aHostPtr, MemoryObjectWrapper** aResultOut);

    cl_int createSampler (cl_bool aNormalizedCoords,
                          cl_addressing_mode aAdressingMode,
                          cl_filter_mode aFilterMode,
                          SamplerWrapper** aResultOut);

    cl_int getSupportedImageFormats (cl_mem_flags aFlags,
                                     cl_mem_object_type aImageType,
                                     std::vector<ImageFormatWrapper>& aResultOut);

    // Note: OpenCL 1.1
    cl_int createUserEvent (EventWrapper** aResultOut);


    // OpenGL interoperability functions
    // Note: OpenCL/OpenGL interop 1.0
    cl_int createFromGLBuffer (cl_mem_flags aFlags,
                               cl_GLuint aGLBufferObject,
                               MemoryObjectWrapper** aResultOut);

    // Note: OpenCL/OpenGL interop 1.0
    cl_int createFromGLTexture2D (cl_mem_flags aFlags,
                                  cl_GLenum aGLTextureTarget,
                                  cl_GLint aMipLevel,
                                  cl_GLuint aTexture,
                                  MemoryObjectWrapper** aResultOut);

    // Note: OpenCL/OpenGL interop 1.0
    cl_int createFromGLTexture3D (cl_mem_flags aFlags,
                                  cl_GLenum aGLTextureTarget,
                                  cl_GLint aMipLevel,
                                  cl_GLuint aTexture,
                                  MemoryObjectWrapper** aResultOut);

    // Note: OpenCL/OpenGL interop 1.0
    cl_int createFromGLRenderbuffer (cl_mem_flags aFlags,
                                     cl_GLuint aRenderBuffer,
                                     MemoryObjectWrapper** aResultOut);

protected:
    virtual ~ContextWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainContext (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseContext (mWrapped); }

private:
    ContextWrapper ();
    cl_context mWrapped;

public:
    static InstanceRegistry<cl_context, ContextWrapper*> instanceRegistry;
    static ContextWrapper* getNewOrExisting (cl_context aHandle);

    static cl_int contextInfoHelper (Wrapper const* aInstance, int aName,
                                     size_t aSize, void* aValueOut, size_t* aSizeOut);

    static cl_int createContext (cl_context_properties* aProperties,
                                 std::vector<DeviceWrapper*>& aDevices,
                                 void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                 void* aNotifyUserData,
                                 ContextWrapper** aCtxOut);
    static cl_int createContext (std::vector<std::pair<cl_context_properties,cl_platform_id> >& aProperties,
                                 std::vector<DeviceWrapper*>& aDevices,
                                 void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                 void* aNotifyUserData,
                                 ContextWrapper** aCtxOut);
    static cl_int createContextFromType (cl_context_properties* aProperties,
                                         cl_device_type aDeviceType,
                                         void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                         void* aNotifyUserData,
                                         ContextWrapper** aCtxOut);
    static cl_int createContextFromType (std::vector<std::pair<cl_context_properties,cl_platform_id> >& aProperties,
                                         cl_device_type aDeviceType,
                                         void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                         void* aNotifyUserData,
                                         ContextWrapper** aCtxOut);
    static cl_int waitForEvents (std::vector<EventWrapper const*> const& aEvents);
    static cl_int unloadCompiler ();
};


#endif // CONTEXTWRAPPER_H
