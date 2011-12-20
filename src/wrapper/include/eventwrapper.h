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

/** \file eventwrapper.h
 * Event wrapper class definition.
 */

#ifndef EVENTWRAPPER_H
#define EVENTWRAPPER_H

#include "clwrappercommon.h"


class EventWrapper : public Wrapper {
public:
    EventWrapper (cl_event aHandle);
    cl_event getWrapped () const { return mWrapped; }

    template <typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, eventInfoHelper);
    }

    template <typename T>
    cl_int getProfilingInfo (int aName, T& aValueOut) {
        return Wrapper::getInfo (aName, aValueOut, eventProfilingInfoHelper);
    }

    // Note: OpenCL 1.1
    cl_int setUserEventStatus (cl_int aExecutionStatus);

    // Note: OpenCL 1.1
    cl_int setEventCallback (cl_int aCommandExecCallbackType,
                             void (CL_CALLBACK *aCallback)(cl_event, cl_int, void*),
                             void* aUserData);

protected:
    virtual ~EventWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainEvent (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseEvent (mWrapped); }

private:
    EventWrapper ();
    cl_event mWrapped;

public:
    static InstanceRegistry<cl_event, EventWrapper*> instanceRegistry;
    static EventWrapper* getNewOrExisting (cl_event aHandle);

    static cl_int eventInfoHelper (Wrapper const* aInstance, int aName,
                                   size_t aSize, void* aValueOut, size_t* aSizeOut);

    static cl_int eventProfilingInfoHelper (Wrapper const* aInstance, int aName,
                                            size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // EVENTWRAPPER_H
