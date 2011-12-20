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

/** \file platformwrapper.cpp
 * Platform wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "platformwrapper.h"
#include "devicewrapper.h"

#include <vector>

using std::vector;


InstanceRegistry<cl_platform_id, PlatformWrapper*> PlatformWrapper::instanceRegistry;


PlatformWrapper::PlatformWrapper (cl_platform_id aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


PlatformWrapper::~PlatformWrapper () {
    instanceRegistry.remove (mWrapped);
}


cl_int PlatformWrapper::getDevices (int aType, std::vector<DeviceWrapper*>& aDevicesOut) const {
    cl_int err = CL_SUCCESS;

    cl_device_id* devices = 0;
    cl_uint num = 0;
    err = clGetDeviceIDs(mWrapped, aType, 0, NULL, &num);
    if (err != CL_SUCCESS) return err;

    devices = (cl_device_id*)malloc (sizeof(cl_device_id) * num);
    if (!devices) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }

    err = clGetDeviceIDs(mWrapped, aType, num, devices, 0);
    if (err != CL_SUCCESS) {
        if (devices)
            free (devices);
        return err;
    }

    aDevicesOut.clear ();
    for (cl_uint i = 0; i < num; ++i) {
        aDevicesOut.push_back (DeviceWrapper::getNewOrExisting (devices[i]));
    }
    return err;
}


/* static */
PlatformWrapper* PlatformWrapper::getNewOrExisting (cl_platform_id aHandle) {
    D_METHOD_START;
    PlatformWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) PlatformWrapper (aHandle);
}


/* static */
cl_int PlatformWrapper::platformInfoHelper (Wrapper const* aInstance, int aName,
                                            size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    PlatformWrapper const* instance = dynamic_cast<PlatformWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetPlatformInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}



/* static */
cl_int PlatformWrapper::getPlatforms (std::vector<PlatformWrapper*>& aPlatformsOut) {
    cl_int err = CL_SUCCESS;

    cl_platform_id* platforms = 0;
    cl_uint num = 0;
    err = clGetPlatformIDs(0, NULL, &num);
    if (err != CL_SUCCESS) return err;

    platforms = (cl_platform_id*)malloc (sizeof(cl_platform_id) * num);
    if (!platforms) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }

    err = clGetPlatformIDs(num, platforms, NULL);
    if (err != CL_SUCCESS) {
        if (platforms)
            free (platforms);
        return err;
    }

    aPlatformsOut.clear ();
    for (cl_uint i = 0; i < num; ++i) {
        aPlatformsOut.push_back (getNewOrExisting (platforms[i]));
    }
    return err;
}

