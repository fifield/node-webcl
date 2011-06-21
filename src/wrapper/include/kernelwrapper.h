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

/** \file kernelwrapper.h
 * Kernel wrapper class definition.
 */

#ifndef KERNELWRAPPER_H
#define KERNELWRAPPER_H

#include "clwrappercommon.h"
#include "devicewrapper.h"


class KernelWrapper : public Wrapper {
public:
    KernelWrapper (cl_kernel aHandle);
    cl_kernel getWrapped () const { return mWrapped; }

    template <typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, kernelInfoHelper);
    }

    template <typename T>
    cl_int getWorkGroupInfo (DeviceWrapper const* aDevice, int aName, T& aValueOut) {
        return Wrapper::getInfo (aDevice, aName, aValueOut, kernelWorkGroupInfoHelper);
    }

    cl_int setArg (cl_uint aIndex, size_t aSize, void* aValue);

protected:
    virtual ~KernelWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainKernel (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseKernel (mWrapped); }

private:
    KernelWrapper ();
    cl_kernel mWrapped;

public:
    static InstanceRegistry<cl_kernel, KernelWrapper*> instanceRegistry;
    static KernelWrapper* getNewOrExisting (cl_kernel aHandle);

    static cl_int kernelInfoHelper (Wrapper const* aInstance, int aName,
                                    size_t aSize, void* aValueOut, size_t* aSizeOut);
    static cl_int kernelWorkGroupInfoHelper (Wrapper const* aInstance, Wrapper const* aExtra,
                                             int aName, size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // KERNELWRAPPER_H
