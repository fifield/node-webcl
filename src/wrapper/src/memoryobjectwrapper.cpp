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

/** \file memoryobjectwrapper.cpp
 * Memory object wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "memoryobjectwrapper.h"


InstanceRegistry<cl_mem, MemoryObjectWrapper*> MemoryObjectWrapper::instanceRegistry;

MemoryObjectWrapper::MemoryObjectWrapper (cl_mem aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


MemoryObjectWrapper::~MemoryObjectWrapper () {
    instanceRegistry.remove (mWrapped);
}


cl_int MemoryObjectWrapper::createSubBuffer (cl_mem_flags aFlags,
                                             RegionWrapper const& aRegion,
                                             MemoryObjectWrapper** aResultOut) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = CL_SUCCESS;
    VALIDATE_ARG_POINTER (aResultOut, &err, err);

    cl_buffer_region region;
    region.origin = aRegion.origin;
    region.size = aRegion.size;

    cl_mem mem = clCreateSubBuffer (mWrapped, aFlags, CL_BUFFER_CREATE_TYPE_REGION,
                                 (void const*)&region, &err);
    if (err != CL_SUCCESS || !mem)
        D_LOG (LOG_LEVEL_ERROR, "clCreateSubBuffer failed. (error %d)", err);

    // NOTE: clCreateSubBuffer can return an existing handle.
    *aResultOut = MemoryObjectWrapper::getNewOrExisting (mem);
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aFlags; (void)aRegion; (void)aResultOut;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif
}


cl_int MemoryObjectWrapper::getGLObjectInfo (cl_gl_object_type *aGLObjectTypeOut,
                                             cl_GLuint *aGLObjectNameOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    D_METHOD_START;
    cl_int err = clGetGLObjectInfo (mWrapped, aGLObjectTypeOut, aGLObjectNameOut);
    if (err != CL_SUCCESS)
        D_LOG (LOG_LEVEL_ERROR, "clGetGLObjectInfo failed. (error %d)", err);
    return err;
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aGLObjectTypeOut; (void)aGLObjectNameOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


/* static */
MemoryObjectWrapper* MemoryObjectWrapper::getNewOrExisting (cl_mem aHandle) {
    D_METHOD_START;
    MemoryObjectWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) MemoryObjectWrapper (aHandle);
}


/* static */
cl_int MemoryObjectWrapper::memoryObjectInfoHelper (Wrapper const* aInstance, int aName,
                                                    size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    MemoryObjectWrapper const* instance = dynamic_cast<MemoryObjectWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetMemObjectInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}


/* static */
cl_int MemoryObjectWrapper::imageInfoHelper (Wrapper const* aInstance, int aName,
                                             size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    MemoryObjectWrapper const* instance = dynamic_cast<MemoryObjectWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetImageInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}


/* static */
cl_int MemoryObjectWrapper::GLTextureInfo (Wrapper const* aInstance, int aName,
                                           size_t aSize, void* aValueOut, size_t* aSizeOut) {
#ifdef CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    cl_int err = CL_SUCCESS;
    MemoryObjectWrapper const* instance = dynamic_cast<MemoryObjectWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetGLTextureInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
#else //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
    (void)aInstance; (void)aName; (void)aSize; (void)aValueOut; (void)aSizeOut;
    D_LOG (LOG_LEVEL_ERROR,
           "CLWrapper support for OpenCL/OpenGL interoperability API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif //CL_WRAPPER_ENABLE_OPENGL_SUPPORT
}


