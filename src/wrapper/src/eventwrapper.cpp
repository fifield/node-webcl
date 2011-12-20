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

/** \file eventwrapper.cpp
 * Event wrapper class implementation.
 */

#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"
#include "eventwrapper.h"


InstanceRegistry<cl_event, EventWrapper*> EventWrapper::instanceRegistry;

EventWrapper::EventWrapper (cl_event aHandle)
    : Wrapper (),
      mWrapped (aHandle)
{
    instanceRegistry.add (aHandle, this);
}


EventWrapper::~EventWrapper () {
    instanceRegistry.remove (mWrapped);
}


cl_int EventWrapper::setUserEventStatus (cl_int aExecutionStatus) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    D_METHOD_START;
    cl_int err = clSetUserEventStatus (mWrapped, aExecutionStatus);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, " clSetUserEventStatus failed. (error %d)", err);
    }
    return err;
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aExecutionStatus;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif
}


cl_int EventWrapper::setEventCallback (cl_int aCommandExecCallbackType,
                                       void (CL_CALLBACK *aCallback)(cl_event, cl_int, void*),
                                       void* aUserData) {
#if CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    return clSetEventCallback (mWrapped, aCommandExecCallbackType,
                               aCallback, aUserData);
#else // CL_WRAPPER_CL_VERSION_SUPPORT >= 110
    (void)aCommandExecCallbackType; (void)aCallback; (void)aUserData;
    D_LOG (LOG_LEVEL_ERROR, "CLWrapper support for OpenCL 1.1 API was not enabled at build time.");
    return CL_INVALID_VALUE;
#endif
}


/* static */
EventWrapper* EventWrapper::getNewOrExisting (cl_event aHandle) {
    D_METHOD_START;
    EventWrapper* res = 0;
    if (instanceRegistry.findById (aHandle, &res) && res) {
        res->retain ();
        return res;
    }
    return new(std::nothrow) EventWrapper (aHandle);
}


/* static */
cl_int EventWrapper::eventInfoHelper (Wrapper const* aInstance, int aName,
                                      size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    EventWrapper const* instance = dynamic_cast<EventWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetEventInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}

/* static */
cl_int EventWrapper::eventProfilingInfoHelper (Wrapper const* aInstance, int aName,
                                               size_t aSize, void* aValueOut, size_t* aSizeOut) {
    cl_int err = CL_SUCCESS;
    EventWrapper const* instance = dynamic_cast<EventWrapper const*>(aInstance);
    VALIDATE_ARG_POINTER (instance, &err, err);
    return clGetEventProfilingInfo (instance->getWrapped (), aName, aSize, aValueOut, aSizeOut);
}


