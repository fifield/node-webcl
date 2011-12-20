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

/** \file memoryobjectwrapper.h
 * Memory object wrapper class definition.
 */

#ifndef MEMORYOBJECTWRAPPER_H
#define MEMORYOBJECTWRAPPER_H

#include "clwrappercommon.h"

class MemoryObjectWrapper : public Wrapper {
public:
    MemoryObjectWrapper (cl_mem aHandle);
    cl_mem getWrapped () const { return mWrapped; }

    // Note: OpenCL 1.1
    cl_int createSubBuffer (cl_mem_flags aFlags,
                            RegionWrapper const& aRegion,
                            MemoryObjectWrapper** aResultOut);

    template <typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, memoryObjectInfoHelper);
    }

    template <typename T>
    cl_int getImageInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, imageInfoHelper);
    }

    // Note: OpenCL/OpenGL interop 1.0
    template <typename T>
    cl_int getGLTextureInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, GLTextureInfo);
    }

    // Note: OpenCL/OpenGL interop 1.0
    cl_int getGLObjectInfo (cl_gl_object_type *aGLObjectTypeOut,
                            cl_GLuint *aGLObjectNameOut);

protected:
    virtual ~MemoryObjectWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainMemObject (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseMemObject (mWrapped); }

private:
    MemoryObjectWrapper ();
    cl_mem mWrapped;

public:
    static InstanceRegistry<cl_mem, MemoryObjectWrapper*> instanceRegistry;
    static MemoryObjectWrapper* getNewOrExisting (cl_mem aHandle);

    static cl_int memoryObjectInfoHelper (Wrapper const* aInstance, int aName,
                                          size_t aSize, void* aValueOut, size_t* aSizeOut);
    static cl_int imageInfoHelper (Wrapper const* aInstance, int aName,
                                   size_t aSize, void* aValueOut, size_t* aSizeOut);
    // Note: OpenCL/OpenGL interop 1.0
    static cl_int GLTextureInfo (Wrapper const* aInstance, int aName,
                                 size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // MEMORYOBJECTWRAPPER_H
