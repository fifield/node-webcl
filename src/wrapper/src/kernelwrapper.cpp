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

/** \file kernelwrapper.cpp
 * Kernel wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "kernelwrapper.h"
#include "devicewrapper.h"
#include "clwrappertypes.h"

#include <vector>
#include <string>
using std::vector;
using std::string;


InstanceRegistry<cl_kernel, KernelWrapper*> KernelWrapper::instanceRegistry;

KernelWrapper::KernelWrapper (cl_kernel aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


KernelWrapper::~KernelWrapper () {
    instanceRegistry.remove (mWrapped);
}


cl_int KernelWrapper::setArg (cl_uint aIndex, size_t aSize, void* aValue) {
    D_METHOD_START;
    cl_int err = clSetKernelArg (mWrapped, aIndex, aSize, aValue);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "clSetKernelArg failed. (error %d)", err);
    }
    return err;
}


/* static */
KernelWrapper* KernelWrapper::getNewOrExisting (cl_kernel aHandle) {
    D_METHOD_START;
    KernelWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) KernelWrapper (aHandle);
}


/* static */
cl_int KernelWrapper::kernelInfoHelper (Wrapper const* aInstance, int aName,
                                        size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    KernelWrapper const* instance = dynamic_cast<KernelWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetKernelInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}


/* static */
cl_int KernelWrapper::kernelWorkGroupInfoHelper (Wrapper const* aInstance, Wrapper const* aExtra,
                                                 int aName, size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    KernelWrapper const* instance = dynamic_cast<KernelWrapper const*>(aInstance);
    DeviceWrapper const* device = dynamic_cast<DeviceWrapper const*>(aExtra);
    VALIDATE_ARG_POINTER (instance, &err, err);
    VALIDATE_ARG_POINTER (device, &err, err);
    return clGetKernelWorkGroupInfo (instance->getWrapped (), device->getWrapped (),
                                     aName, aSize, aValueOut, aSizeOut);
}

