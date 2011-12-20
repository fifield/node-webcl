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

/** \file contextwrapper.cpp
 * Context wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "contextwrapper.h"
#include "programwrapper.h"
#include "devicewrapper.h"
#include "commandqueuewrapper.h"
#include "memoryobjectwrapper.h"
#include "samplerwrapper.h"
#include "platformwrapper.h"
#include "eventwrapper.h"

#include <string>
#include <utility>
#include <vector>

using std::string;
using std::pair;
using std::vector;


InstanceRegistry<cl_context, ContextWrapper*> ContextWrapper::instanceRegistry;


ContextWrapper::ContextWrapper (cl_context aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


ContextWrapper::~ContextWrapper () {
    instanceRegistry.remove (mWrapped);
}


cl_int ContextWrapper::createProgramWithSource (std::string const& aSource,
                                                ProgramWrapper** aProgramOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aProgramOut, &err, err);

    char const* src = aSource.c_str ();
    cl_program program = clCreateProgramWithSource (mWrapped, 1,
                                                    &src,
                                                    NULL, &err);
    if (err != CL_SUCCESS || !program)
        D_LOG (LOG_LEVEL_ERROR, "clCreateProgramWithSource failed (error %d)", err);

    *aProgramOut = ProgramWrapper::getNewOrExisting (program);
    return err;
}


cl_int ContextWrapper::createProgramWithBinary (std::vector<DeviceWrapper*> const& aDevices,
                                                std::vector<std::string const*> const& aBinaries,
                                                std::vector<cl_int>& aBinaryStatusOut,
                                                ProgramWrapper** aProgramOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aProgramOut, &err, err);

    cl_uint deviceListLen = 0;
    cl_device_id* deviceList = 0;
    size_t* binaryLengthList = 0;
    unsigned char const** binaryList = 0;
    cl_int* binaryStatusList = 0;
    cl_program program;

    deviceListLen = aDevices.size ();

    // Ensure all vectors have the same length, i.e. length of devices vector
    if (aBinaries.size () != deviceListLen) {
        D_LOG (LOG_LEVEL_ERROR, "Length of aBinaries does not match aDevices,");
        err = CL_INVALID_VALUE;
        goto error;
    }
    if (aBinaryStatusOut.size () != deviceListLen) {
        D_LOG (LOG_LEVEL_ERROR, "Length of aBinaryStatusOut does not match aDevices,");
        err = CL_INVALID_VALUE;
        goto error;
    }

    deviceList = (cl_device_id*)malloc (sizeof (cl_device_id) * deviceListLen);
    if (deviceList) {
        vector<DeviceWrapper*>::const_iterator i = aDevices.begin ();
        size_t cnt = 0;
        while (i != aDevices.end ()) {
            deviceList[cnt++] = (*i)->getWrapped ();
            ++i;
        }
    } else {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        err = CL_OUT_OF_HOST_MEMORY;
        goto error;
    }

    binaryLengthList = (size_t*)malloc (sizeof(size_t) * deviceListLen);
    binaryList = (unsigned char const**)malloc (sizeof (unsigned char*) * deviceListLen);
    if (binaryLengthList && binaryList) {
        vector<std::string const*>::const_iterator i = aBinaries.begin ();
        size_t cnt = 0;
        while (i != aBinaries.end ()) {
            if (!(*i)) {
                err = CL_INVALID_VALUE;
                goto error;
            }
            binaryLengthList[cnt] = (*i)->size ();
            binaryList[cnt] = (unsigned char const*)((*i)->c_str ());
            ++cnt;
            ++i;
        }
    } else {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        err = CL_OUT_OF_HOST_MEMORY;
        goto error;
    }

    binaryStatusList = (cl_int*)malloc (sizeof(cl_int) * deviceListLen);
    if (!binaryStatusList) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        err = CL_OUT_OF_HOST_MEMORY;
        goto error;
    }

    program = clCreateProgramWithBinary (mWrapped,
                                         deviceListLen, deviceList,
                                         binaryLengthList,
                                         binaryList,
                                         binaryStatusList,
                                         &err);
    if (CL_FAILED (err) || !program) {
        D_LOG (LOG_LEVEL_ERROR, "clCreateProgramWithSource failed (error %d)", err);
        goto error;
    }

    *aProgramOut = ProgramWrapper::getNewOrExisting (program);

    aBinaryStatusOut.clear ();
    for (cl_uint i = 0; i < deviceListLen; ++i)
        aBinaryStatusOut.push_back (binaryStatusList[i]);

error:
    if (deviceList) free (deviceList);
    if (binaryLengthList) free (binaryLengthList);
    if (binaryList) free (binaryList);
    if (binaryStatusList) free (binaryStatusList);

    return err;
}


cl_int ContextWrapper::createCommandQueue (DeviceWrapper* aDevice,
                                           cl_command_queue_properties aProperties,
                                           CommandQueueWrapper** aCmdQueueOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aDevice, &err, err);
    VALIDATE_ARG_POINTER (aCmdQueueOut, &err, err);

    cl_command_queue cmdQ = clCreateCommandQueue (mWrapped,
                                                  aDevice->getWrapped (),
                                                  aProperties, &err);
    if (err != CL_SUCCESS || !cmdQ)
        D_LOG (LOG_LEVEL_ERROR, "clCreateCommandQueue failed. (error %d)", err);

    *aCmdQueueOut = CommandQueueWrapper::getNewOrExisting (cmdQ);
    return err;
}


cl_int ContextWrapper::createBuffer (cl_mem_flags aFlags, size_t aSize,
                                     void* aHostPtr,
                                     MemoryObjectWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_mem mem = clCreateBuffer (mWrapped, aFlags, aSize, aHostPtr, &err);
    if (err != CL_SUCCESS || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateBuffer failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
}


cl_int ContextWrapper::createImage2D (cl_mem_flags aFlags,
                                      ImageFormatWrapper const& aImageFormat,
                                      size_t aWidth,
                                      size_t aHeight,
                                      size_t aRowPitch,
                                      void* aHostPtr,
                                      MemoryObjectWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_image_format format;
    format.image_channel_order = aImageFormat.channelOrder;
    format.image_channel_data_type = aImageFormat.channelDataType;

    cl_mem mem = clCreateImage2D (mWrapped, aFlags, &format,
                                  aWidth, aHeight, aRowPitch, aHostPtr,
                                  &err);
    if (err != CL_SUCCESS || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateImage2D failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
}


cl_int ContextWrapper::createImage3D (cl_mem_flags aFlags,
                                      ImageFormatWrapper const& aImageFormat,
                                      size_t aWidth, size_t aHeight, size_t aDepth,
                                      size_t aRowPitch, size_t aSlicePitch,
                                      void* aHostPtr, MemoryObjectWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_image_format format;
    format.image_channel_order = aImageFormat.channelOrder;
    format.image_channel_data_type = aImageFormat.channelDataType;

    cl_mem mem = clCreateImage3D (mWrapped, aFlags, &format,
                                  aWidth, aHeight, aDepth,
                                  aRowPitch, aSlicePitch,
                                  aHostPtr, &err);
    if (err != CL_SUCCESS || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateImage3D failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
}


cl_int ContextWrapper::createSampler (cl_bool aNormalizedCoords,
                                      cl_addressing_mode aAdressingMode,
                                      cl_filter_mode aFilterMode,
                                      SamplerWrapper** aResultOut) {
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_sampler sampler = clCreateSampler (mWrapped, aNormalizedCoords,
                                          aAdressingMode, aFilterMode, &err);
    if (err != CL_SUCCESS || !sampler)
        D_LOG (LOG_LEVEL_ERROR, "clCreateSampler failed. (error %d)", err);

    *aResultOut = SamplerWrapper::getNewOrExisting (sampler);
    return err;
}


cl_int ContextWrapper::getSupportedImageFormats (cl_mem_flags aFlags,
                                                 cl_mem_object_type aImageType,
                                                 std::vector<ImageFormatWrapper>& aResultOut) {
    D_METHOD_START;

    cl_image_format* buf = 0;
    cl_uint num = 0;
    cl_int err = clGetSupportedImageFormats (mWrapped, aFlags, aImageType,
                                             0, 0, &num);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clGetSupportedImageFormats failed. (error %d)", err);
        return err;
    }

    buf = (cl_image_format*)malloc (sizeof (cl_image_format) * num);
    if (!buf) return CL_OUT_OF_HOST_MEMORY;

    err = clGetSupportedImageFormats (mWrapped, aFlags, aImageType, num, buf, 0);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clGetSupportedImageFormats failed. (error %d)", err);
        return err;
    }

    aResultOut.clear ();
    aResultOut.reserve (num);
    for (cl_uint i = 0; i < num; ++i) {
        aResultOut.push_back (ImageFormatWrapper (buf[i].image_channel_order,
                                                  buf[i].image_channel_data_type));
    }

    return err;
}


cl_int ContextWrapper::createUserEvent (EventWrapper** aResultOut) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_event event = clCreateUserEvent (mWrapped, &err);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clCreateUserEvent failed. (error %d)", err);
        return err;
    }

    *aResultOut = EventWrapper::getNewOrExisting (event);
    if (!*aResultOut) return CL_OUT_OF_HOST_MEMORY;
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
}


cl_int ContextWrapper::createFromGLBuffer (cl_mem_flags aFlags,
                                           cl_GLuint aGLBufferObject,
                                           MemoryObjectWrapper** aResultOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_mem mem = clCreateFromGLBuffer (mWrapped, aFlags, aGLBufferObject, &err);
    if (CL_FAILED (err) || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateFromGLBuffer failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aFlags; (void)aGLBufferObject; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


cl_int ContextWrapper::createFromGLTexture2D (cl_mem_flags aFlags,
                                              cl_GLenum aGLTextureTarget,
                                              cl_GLint aMipLevel,
                                              cl_GLuint aTexture,
                                              MemoryObjectWrapper** aResultOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_mem mem = clCreateFromGLTexture2D (mWrapped, aFlags, aGLTextureTarget,
                                          aMipLevel, aTexture, &err);
    if (CL_FAILED (err) || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateFromGLTexture2D failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aFlags; (void)aGLTextureTarget; (void)aMipLevel; (void)aTexture; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


cl_int ContextWrapper::createFromGLTexture3D (cl_mem_flags aFlags,
                                              cl_GLenum aGLTextureTarget,
                                              cl_GLint aMipLevel,
                                              cl_GLuint aTexture,
                                              MemoryObjectWrapper** aResultOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_mem mem = clCreateFromGLTexture3D (mWrapped, aFlags, aGLTextureTarget,
                                          aMipLevel, aTexture, &err);
    if (CL_FAILED (err) || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateFromGLTexture3D failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aFlags; (void)aGLTextureTarget; (void)aMipLevel; (void)aTexture; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


cl_int ContextWrapper::createFromGLRenderbuffer (cl_mem_flags aFlags,
                                                 cl_GLuint aRenderBuffer,
                                                 MemoryObjectWrapper** aResultOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_mem mem = clCreateFromGLRenderbuffer (mWrapped, aFlags, aRenderBuffer, &err);
    if (CL_FAILED (err) || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateFromGLRenderbuffer failed. (error %d)", err);

    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aFlags; (void)aRenderBuffer; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}



/* static */
ContextWrapper* ContextWrapper::getNewOrExisting (cl_context aHandle) {
    D_METHOD_START;
    ContextWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) ContextWrapper (aHandle);
}


/* static */
cl_int ContextWrapper::contextInfoHelper (Wrapper const* aInstance, int aName,
                                          size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    ContextWrapper const* instance = dynamic_cast<ContextWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetContextInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}


/* static */
cl_int ContextWrapper::createContext (cl_context_properties* aProperties,
                                      std::vector<DeviceWrapper*>& aDevices,
                                      void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                      void* aNotifyUserData,
                                      ContextWrapper** aCtxOut) {
    D_METHOD_START;

    cl_device_id* devices = new(std::nothrow) cl_device_id [aDevices.size()];
    if (!devices) {
        return CL_OUT_OF_HOST_MEMORY;
    }

    size_t idx = 0;
    vector<DeviceWrapper*>::const_iterator i = aDevices.begin ();
    while (i != aDevices.end ()) {
        if (*i) {
            devices [idx++] = (*i)->getWrapped ();
        }
        ++i;
    }


    cl_int err = CL_SUCCESS;
    cl_context ctx = clCreateContext(aProperties, aDevices.size(), devices,
                                     aNotify, aNotifyUserData, &err);


    delete[] devices;

    if (aCtxOut)
        *aCtxOut = ContextWrapper::getNewOrExisting (ctx);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clCreateContext failed with error %d.", err);
    }

    return err;
}


/* static */
cl_int ContextWrapper::createContext (std::vector<std::pair<cl_context_properties,cl_platform_id> >& aProperties,
                                      std::vector<DeviceWrapper*>& aDevices,
                                      void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                      void* aNotifyUserData,
                                      ContextWrapper** aCtxOut) {
    D_METHOD_START;
    cl_context_properties* props = new(std::nothrow) cl_context_properties [2 * aProperties.size() + 2 + 1];
    if (!props) return CL_OUT_OF_HOST_MEMORY;

    size_t idx = 0;
    vector<pair<cl_context_properties,cl_platform_id> >::const_iterator i = aProperties.begin ();
    while (i != aProperties.end ()) {
        props [idx++] = i->first;
        props [idx++] = (cl_context_properties)i->second;
        ++i;
    }
    props[idx] = 0;


    cl_int err = createContext (props, aDevices, aNotify, aNotifyUserData, aCtxOut);
    delete[] props;


    return err;
}


/* static */
cl_int ContextWrapper::createContextFromType (cl_context_properties* aProperties,
                                              cl_device_type aDeviceType,
                                              void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                              void* aNotifyUserData,
                                              ContextWrapper** aCtxOut) {
    D_METHOD_START;

    cl_int err = CL_SUCCESS;
    cl_context ctx = clCreateContextFromType(aProperties, aDeviceType,
                                             aNotify, aNotifyUserData, &err);

    if (aCtxOut)
        *aCtxOut = ContextWrapper::getNewOrExisting (ctx);

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clCreateContextFromType failed. (error %d)", err);
    }

    return err;
}


/* static */
cl_int ContextWrapper::createContextFromType (std::vector<std::pair<cl_context_properties,cl_platform_id> >& aProperties,
                                              cl_device_type aDeviceType,
                                              void (CL_CALLBACK *aNotify) (const char *, const void *, size_t cb, void *),
                                              void* aNotifyUserData,
                                              ContextWrapper** aCtxOut) {
    D_METHOD_START;

    cl_context_properties* props = new(std::nothrow) cl_context_properties [2 * aProperties.size() + 2 + 1];
    if (!props) return CL_OUT_OF_HOST_MEMORY;

    size_t idx = 0;
    vector<pair<cl_context_properties,cl_platform_id> >::const_iterator i = aProperties.begin ();
    while (i != aProperties.end ()) {
        props [idx++] = i->first;
        props [idx++] = (cl_context_properties)i->second;
        ++i;
    }

    props[idx] = 0;


    cl_int err = createContextFromType (props, aDeviceType, aNotify,
                                        aNotifyUserData, aCtxOut);


    delete[] props;

    return err;
}


/* static */
cl_int ContextWrapper::waitForEvents (std::vector<EventWrapper const*> const& aEvents) {
    cl_event* evlist = new(std::nothrow) cl_event[aEvents.size ()];
    if (!evlist) return CL_OUT_OF_HOST_MEMORY;

    cl_uint cnt = 0;
    for (vector<EventWrapper const*>::const_iterator i = aEvents.begin (); i != aEvents.end (); ++i) {
        if (*i) {
            evlist[cnt] = (*i)->getWrapped ();
            ++cnt;
        }
    }

    cl_int err = clWaitForEvents (cnt, evlist);
    delete[] evlist;

    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clWaitForEvents failed. (error %d)", err);
    }

    return err;
}


/* static */
cl_int ContextWrapper::unloadCompiler () {
    return clUnloadCompiler ();
}
