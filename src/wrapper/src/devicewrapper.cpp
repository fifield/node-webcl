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

/** \file devicewrapper.cpp
 * Device wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "devicewrapper.h"


InstanceRegistry<cl_device_id, DeviceWrapper*> DeviceWrapper::instanceRegistry;


DeviceWrapper::DeviceWrapper (cl_device_id aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


DeviceWrapper::~DeviceWrapper () {
    instanceRegistry.remove (mWrapped);
}


/* static */
DeviceWrapper* DeviceWrapper::getNewOrExisting (cl_device_id aHandle) {
    D_METHOD_START;
    DeviceWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) DeviceWrapper (aHandle);
}


/* static */
cl_int DeviceWrapper::deviceInfoHelper (Wrapper const* aInstance, int aName,
                                        size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    DeviceWrapper const* instance = dynamic_cast<DeviceWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetDeviceInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}
