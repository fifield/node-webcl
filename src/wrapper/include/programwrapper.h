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

/** \file programwrapper.h
 * Program wrapper class definition.
 */

#ifndef PROGRAMWRAPPER_H
#define PROGRAMWRAPPER_H

#include "clwrappercommon.h"
#include "devicewrapper.h"

#include <vector>
#include <string>

class KernelWrapper;
class ProgramWrapper;

/* Namespace ProgramWrapperInfoDetail contains templates to separate
 * CL_PROGRAM_BINARIES info requests and to divert them to local getInfo
 * implementation.
 * ProgramWrapperInfoDetail::getProgramBinariesInfo forwards call to
 * ProgramWrapper::getProgramBinariesInfo thus enabling calls to
 * protected members in Wrapper. This is needed due to otherwise circular
 * dependencies between ProgramWrapperInfoDetail and ProgramWrapper.
 */
namespace ProgramWrapperInfoDetail {
    cl_int getProgramBinariesInfo (ProgramWrapper* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc);

    template <typename T> inline cl_int getInfo (ProgramWrapper* aInstance, int aName, T& aValueOut, InfoFunc infoFunc) {
        Wrapper* w = (Wrapper*)aInstance;
        if (w) {
            return CLWrapperDetail::getInfo (w, aName, aValueOut, infoFunc);
        } else {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Internal error! Failed to cast instance to Wrapper!");
            return CL_INVALID_VALUE;
        }
    }

    // Specialization for string vectors, if aName is CL_PROGRAM_BINARIES a
    // special version of getProgramInfo is called.
    template <> inline cl_int getInfo<std::vector<std::string> > (ProgramWrapper* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc) {
        Wrapper* w = (Wrapper*)aInstance;
        if (w) {
            if (aName == CL_PROGRAM_BINARIES) {
                return getProgramBinariesInfo (aInstance, aName, aValueOut, infoFunc);
            } else {
                return CLWrapperDetail::getInfo (w, aName, aValueOut, infoFunc);
            }
        }else {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Internal error! Failed to cast instance to Wrapper!");
            return CL_INVALID_VALUE;
        }
    }
};

class ProgramWrapper : public Wrapper {
public:
    friend cl_int ProgramWrapperInfoDetail::getProgramBinariesInfo (ProgramWrapper* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc);

    ProgramWrapper (cl_program aHandle);
    cl_program getWrapped () const { return mWrapped; }

    template <typename T>
    cl_int getInfo (int aName, T& aValueOut) {
        /* Instead of calling the usual Wrapper::getInfo, ProgramWrapper has
         * a local implementation as templates in ProgramWrapperInfoDetail.
         * This is necessary to handle CL_PROGRAM_BINARIES.
         */
        return ProgramWrapperInfoDetail::getInfo (this, aName, aValueOut, programInfoHelper);
    }

    template <typename T>
    cl_int getBuildInfo (DeviceWrapper const* aDevice, int aParamName, T& aValueOut) {
        return Wrapper::getInfo (aDevice, aParamName, aValueOut, programBuildInfoHelper);
    }

    cl_int buildProgram (std::vector<DeviceWrapper*> const& aDevices,
                         std::string aOptions,
                         void (CL_CALLBACK *aNotify)(cl_program, void*),
                         void* aNotifyUserData);

    cl_int createKernel (std::string aKernelName, KernelWrapper** aResultOut);
    cl_int createKernelsInProgram (std::vector<KernelWrapper*>& aResultOut);

protected:
    virtual ~ProgramWrapper ();
    virtual inline cl_int retainWrapped () const { return clRetainProgram (mWrapped); }
    virtual inline cl_int releaseWrapped () const { return clReleaseProgram (mWrapped); }

    /* This function retrieves and returns the value of CL_PROGRAM_BINARIES
     * info parameter. A separate function is necessary since CL_PROGRAM_BINARIES
     * returns an array of char pointers, sizes of which must be obtained
     * with CL_PROGRAM_BINARY_SIZES.
     *
     * TODO: std::string is not an optimal choice for storing program binaries.
     *       Maybe std::vector<uint8_t> ?
     */
    cl_int getProgramBinariesInfo (int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc) const;

private:
    ProgramWrapper ();
    cl_program mWrapped;

public:
    static InstanceRegistry<cl_program, ProgramWrapper*> instanceRegistry;
    static ProgramWrapper* getNewOrExisting (cl_program aHandle);

    static cl_int programInfoHelper (Wrapper const* aInstance, int aName,
                                     size_t aSize, void* aValueOut, size_t* aSizeOut);
    static cl_int programBuildInfoHelper (Wrapper const* aInstance, Wrapper const* aExtra,
                                          int aName, size_t aSize, void* aValueOut, size_t* aSizeOut);
};

#endif // PROGRAMWRAPPER_H
