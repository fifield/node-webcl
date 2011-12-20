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

/** \file samplerwrapper.cpp
 * Sampler wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "samplerwrapper.h"


InstanceRegistry<cl_sampler, SamplerWrapper*> SamplerWrapper::instanceRegistry;

SamplerWrapper::SamplerWrapper (cl_sampler aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


SamplerWrapper::~SamplerWrapper () {
    instanceRegistry.remove (mWrapped);
}


/* static */
SamplerWrapper* SamplerWrapper::getNewOrExisting (cl_sampler aHandle) {
    D_METHOD_START;
    SamplerWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) SamplerWrapper (aHandle);
}


/* static */
cl_int SamplerWrapper::samplerInfoHelper (Wrapper const* aInstance, int aName,
                                          size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    SamplerWrapper const* instance = dynamic_cast<SamplerWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetSamplerInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}

