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

/** \file devicewrapper.h
 * Device wrapper class definition.
 */

#ifndef DEVICEWRAPPER_H
#define DEVICEWRAPPER_H

#include "clwrappercommon.h"


class DeviceWrapper : public Wrapper {
public:
    DeviceWrapper (cl_device_id aHandle);
    cl_device_id getWrapped () const { return mWrapped; }

    template<typename T> cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, deviceInfoHelper);
    }

protected:
    virtual ~DeviceWrapper ();
    virtual cl_int retainWrapped () const { return CL_SUCCESS; } // No retain for cl_device_id
    virtual cl_int releaseWrapped () const { return CL_SUCCESS; } // No release for cl_device_id

private:
    DeviceWrapper ();
    cl_device_id mWrapped;

public:
    static DeviceWrapper* getNewOrExisting (cl_device_id aHandle);
    static InstanceRegistry<cl_device_id, DeviceWrapper*> instanceRegistry;

    static cl_int deviceInfoHelper (Wrapper const* aInstance, int aName,
                                    size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // DEVICEWRAPPER_H
