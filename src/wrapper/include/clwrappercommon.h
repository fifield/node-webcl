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

/** \file clwrappercommon.h
 * This file contains class definition of Wrapper, the common base class for 
 * wrapper classes, as well as two minor classes ImageFormatWrapper and
 * RegionWrapper.
 * Also included namespace CLWrapperDetail contains a number of
 * getInfo-helper templates and utilities.
 */

#ifndef CLWRAPPERCOMMON_H
#define CLWRAPPERCOMMON_H

#include "CL/cl.h"
#include "CL/cl_platform.h"
#include "CL/cl_gl.h"

#include "instance_registry.h"

#include <cstdlib>
#include <map>
#include <vector>
#include <string>

#include <cstdint>


/** CL_SUCCEEDED evaluates to true if the OpenCL error value \c err
 * indicates successfull operation.
 */
#define CL_SUCCEEDED(err) (err == CL_SUCCESS)

/** CL_FAILED evaluates to true if the OpenCL error value \c err
 * indicates failed operation.
 */
#define CL_FAILED(err) (err != CL_SUCCESS)


// Forward declarations required for wrapper class -specific getInfo functions.
class Wrapper;
class PlatformWrapper;
class DeviceWrapper;
class ContextWrapper;
class EventWrapper;
class CommandQueueWrapper;
class MemoryObjectWrapper;
class ProgramWrapper;
class KernelWrapper;
class SamplerWrapper;


// Wrapper's getInfo <-> OpenCL getXInfo
typedef cl_int (*InfoFunc)(Wrapper const* aInstance, int aName, size_t aSize, void* aValueOut, size_t* aSizeOut);
typedef cl_int (*InfoFuncExtra)(Wrapper const* aInstance, Wrapper const* aExtra, int aName, size_t aSize, void* aValueOut, size_t* aSizeOut);


// We don't want to expose logging mechanism to headers due to conflicts on
// 
// The internal logging mechanism (D_LOG etc.) should not be exposed by
// externally available headers. However, there are some template functions
// in the headers that benefit from logging. The CL_W_LOGGER macro defined
// below is to be used in those cases.
#ifdef WIN32
#define CL_W_LOGGER(level,...)  CLWrapperDetail::logger (__FILE__, __LINE__, __FUNCTION__, level, __VA_ARGS__);
#else // WIN32
#define CL_W_LOGGER(level,msg...)  CLWrapperDetail::logger (__FILE__, __LINE__, __FUNCTION__, level, msg);
#endif // WIN32

// Log levels are not available for externally used headers since
// clwrappercommon_internal is not included.
#ifdef CLWRAPPERCOMMON_INTERNAL_H
# define CL_W_LOG_LEVEL_NONE      LOG_LEVEL_NONE
# define CL_W_LOG_LEVEL_ERROR     LOG_LEVEL_ERROR
# define CL_W_LOG_LEVEL_WARNING   LOG_LEVEL_WARNING
# define CL_W_LOG_LEVEL_INFO      LOG_LEVEL_INFO
# define CL_W_LOG_LEVEL_DEBUG     LOG_LEVEL_DEBUG
#else //CLWRAPPERCOMMON_INTERNAL_H
# define CL_W_LOG_LEVEL_NONE      0
# define CL_W_LOG_LEVEL_ERROR     1
# define CL_W_LOG_LEVEL_WARNING   2
# define CL_W_LOG_LEVEL_INFO      3
# define CL_W_LOG_LEVEL_DEBUG     4
#endif //CLWRAPPERCOMMON_INTERNAL_H


/* Explicit template specialization is not allowed within class, even though
 * some compilers allow it. We place our getInfo-related templates and their
 * specializations in CLWrapperDetail namespace
 */
namespace CLWrapperDetail {
    /* We don't want to expose our local common.h where logging macros reside so
       the logger function, introduced here and implemented in clwrappercommon.cpp,
       is used instead. This function should be invoked via CL_W_LOGGER macro. */
    void logger (char const* file, unsigned int line, char const* function,
                 int level, char const* msg, ...);

    // GetInfo implementations for strings
    cl_int getInfoImpl_string (Wrapper const* aInstance, int aName, std::string& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_string_V (Wrapper const* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc);

    // GetInfo implementations functions for wrapper classes
    cl_int getInfoImpl_PlatformWrapper (Wrapper const* aInstance, int aName, PlatformWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_DeviceWrapper (Wrapper const* aInstance, int aName, DeviceWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_ContextWrapper (Wrapper const* aInstance, int aName, ContextWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_EventWrapper (Wrapper const* aInstance, int aName, EventWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_CommandQueueWrapper (Wrapper const* aInstance, int aName, CommandQueueWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_MemoryObjectWrapper (Wrapper const* aInstance, int aName, MemoryObjectWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_ProgramWrapper (Wrapper const* aInstance, int aName, ProgramWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_KernelWrapper (Wrapper const* aInstance, int aName, KernelWrapper*& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_SamplerWrapper (Wrapper const* aInstance, int aName, SamplerWrapper*& aValueOut, InfoFunc infoFunc);

    cl_int getInfoImpl_PlatformWrapper_V (Wrapper const* aInstance, int aName, std::vector<PlatformWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_DeviceWrapper_V (Wrapper const* aInstance, int aName, std::vector<DeviceWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_ContextWrapper_V (Wrapper const* aInstance, int aName, std::vector<ContextWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_EventWrapper_V (Wrapper const* aInstance, int aName, std::vector<EventWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_CommandQueueWrapper_V (Wrapper const* aInstance, int aName, std::vector<CommandQueueWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_MemoryObjectWrapper_V (Wrapper const* aInstance, int aName, std::vector<MemoryObjectWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_ProgramWrapper_V (Wrapper const* aInstance, int aName, std::vector<ProgramWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_KernelWrapper_V (Wrapper const* aInstance, int aName, std::vector<KernelWrapper*>& aValueOut, InfoFunc infoFunc);
    cl_int getInfoImpl_SamplerWrapper_V (Wrapper const* aInstance, int aName, std::vector<SamplerWrapper*>& aValueOut, InfoFunc infoFunc);

    // GetInfo implementations with extra for strings
    cl_int getInfoImpl_string (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::string& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_string_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<std::string>& aValueOut, InfoFuncExtra infoFunc);

    // GetInfo implementations functions with extra for wrapper classes
    cl_int getInfoImpl_PlatformWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, PlatformWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_DeviceWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, DeviceWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_ContextWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, ContextWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_EventWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, EventWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_CommandQueueWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, CommandQueueWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_MemoryObjectWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, MemoryObjectWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_ProgramWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, ProgramWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_KernelWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, KernelWrapper*& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_SamplerWrapper (Wrapper const* aInstance, Wrapper const* aExtra, int aName, SamplerWrapper*& aValueOut, InfoFuncExtra infoFunc);

    cl_int getInfoImpl_PlatformWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<PlatformWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_DeviceWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<DeviceWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_ContextWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<ContextWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_EventWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<EventWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_CommandQueueWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<CommandQueueWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_MemoryObjectWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<MemoryObjectWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_ProgramWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<ProgramWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_KernelWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<KernelWrapper*>& aValueOut, InfoFuncExtra infoFunc);
    cl_int getInfoImpl_SamplerWrapper_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<SamplerWrapper*>& aValueOut, InfoFuncExtra infoFunc);


    // Basic types
    template<typename T>
    cl_int getInfo (Wrapper const* aInstance, int aName, T& aValueOut, InfoFunc infoFunc) {
        cl_int err = CL_SUCCESS;
        size_t sze = 0;
        if (!infoFunc) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
            return CL_INVALID_VALUE;
        }
        err = infoFunc(aInstance, aName, sizeof(T), (void*)&aValueOut, &sze);
        if (err != CL_SUCCESS) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        if (sze != sizeof(T)) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR,
                  "getInfo returned a value of unexpected size %u, expected (%u bytes)",
                  sze, sizeof(T));
            CL_W_LOGGER (CL_W_LOG_LEVEL_WARNING, "Returning synthetic error value.");
            return CL_INVALID_VALUE;
        }
        return err;
    }

    // Vectors of basic types
    template<typename T>
    cl_int getInfo_basicV (Wrapper const* aInstance, int aName, std::vector<T>& aValueOut, InfoFunc infoFunc) {
        T* buf;
        size_t sze = 0;
        cl_int err = CL_SUCCESS;
        if (!infoFunc) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
            return CL_INVALID_VALUE;
        }
        err = infoFunc (aInstance, aName, 0, 0, &sze);
        if (err != CL_SUCCESS) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        buf = (T*)malloc (sze);
        if (!buf) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Memory allocation failed.");
            CL_W_LOGGER (CL_W_LOG_LEVEL_WARNING, "Returning synthetic error value.");
            return CL_OUT_OF_HOST_MEMORY;
        }
        err = infoFunc (aInstance, aName, sze, (void*)buf, 0);
        if (err != CL_SUCCESS) {
            free (buf);
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        aValueOut.clear ();
        size_t num = sze / sizeof (T);
        aValueOut.reserve (num);
        for (size_t i = 0; i < num; ++i)
            aValueOut.push_back (buf[i]);
        free (buf);
        return err;
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<int8_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<uint8_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<int16_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<uint16_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<int32_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<uint32_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<int64_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<uint64_t>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<float>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<double>& aValueOut, InfoFunc infoFunc) {
        return getInfo_basicV (aInstance, aName, aValueOut, infoFunc);
    }

    // Explicit specializations for strings and string vectors
    template <> inline
    cl_int getInfo<std::string> (Wrapper const* aInstance, int aName, std::string& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_string (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo<std::vector<std::string> > (Wrapper const* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_string_V (aInstance, aName, aValueOut, infoFunc);
    }

    // Explicit specializations for Wrapper-derived objects
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, PlatformWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_PlatformWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, DeviceWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_DeviceWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, ContextWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_ContextWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, EventWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_EventWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, CommandQueueWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_CommandQueueWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, MemoryObjectWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_MemoryObjectWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, ProgramWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_ProgramWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, KernelWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_KernelWrapper (aInstance, aName, aValueOut, infoFunc);
    }
    template <> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, SamplerWrapper*& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_SamplerWrapper (aInstance, aName, aValueOut, infoFunc);
    }

    // Explicit specializations for vectors of Wrapper-derived objects
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<PlatformWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_PlatformWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<DeviceWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_DeviceWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<ContextWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_ContextWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<EventWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_EventWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<CommandQueueWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_CommandQueueWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<MemoryObjectWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_MemoryObjectWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<ProgramWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_ProgramWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<KernelWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_KernelWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, int aName, std::vector<SamplerWrapper*>& aValueOut, InfoFunc infoFunc) {
        return getInfoImpl_SamplerWrapper_V (aInstance, aName, aValueOut, infoFunc);
    }


    // GetInfo with extra for basic types
    template<typename T>
    cl_int getInfo(Wrapper const* aInstance, Wrapper const* aExtra, int aName, T& aValueOut, InfoFuncExtra infoFunc) {
        cl_int err = CL_SUCCESS;
        size_t sze = 0;
        if (!infoFunc) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
            return CL_INVALID_VALUE;
        }
        err = infoFunc(aInstance, aExtra, aName, sizeof(T), (void*)&aValueOut, &sze);
        if (err != CL_SUCCESS) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        if (sze != sizeof(T)) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR,
                    "getInfo returned a value of unexpected size %u, expected (%u bytes)",
                    sze, sizeof(T));
            CL_W_LOGGER (CL_W_LOG_LEVEL_WARNING, "Returning synthetic error value.");
            return CL_INVALID_VALUE;
        }
        return err;
    }

    // GetInfo with extra for vectors of basic types
    template<typename T>
    cl_int getInfo_basicV (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<T>& aValueOut, InfoFuncExtra infoFunc) {
        T* buf;
        size_t sze = 0;
        if (!infoFunc) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
            return CL_INVALID_VALUE;
        }
        cl_int err = infoFunc (aInstance, aExtra, aName, 0, 0, &sze);
        if (err != CL_SUCCESS) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        buf = (T*)malloc (sze);
        if (!buf) {
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "Memory allocation failed.");
            CL_W_LOGGER (CL_W_LOG_LEVEL_WARNING, "Returning synthetic error value.");
            return CL_OUT_OF_HOST_MEMORY;
        }
        err = infoFunc (aInstance, aExtra, aName, sze, (void*)buf, 0);
        if (err != CL_SUCCESS) {
            free (buf);
            CL_W_LOGGER (CL_W_LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
            return err;
        }
        aValueOut.clear ();
        size_t num = sze / sizeof (T);
        aValueOut.reserve (num);
        for (size_t i = 0; i < num; ++i)
            aValueOut.push_back (buf[i]);
        free (buf);
        return err;
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<int8_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<uint8_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<int16_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<uint16_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<int32_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<uint32_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<int64_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<uint64_t>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<float>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<double>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfo_basicV (aInstance, aExtra, aName, aValueOut, infoFunc);
    }

    // GetInfo with extra for strings and string vectors
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::string& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_string (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<std::string>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_string_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }

    // GetInfo with extra for Wrapper-derived objects
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, PlatformWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_PlatformWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, DeviceWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_DeviceWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, ContextWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_ContextWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, EventWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_EventWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, CommandQueueWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_CommandQueueWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, MemoryObjectWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_MemoryObjectWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, ProgramWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_ProgramWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, KernelWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_KernelWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, SamplerWrapper*& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_SamplerWrapper (aInstance, aExtra, aName, aValueOut, infoFunc);
    }

    // GetInfo with extra for vectors of Wrapper-derived objects
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<PlatformWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_PlatformWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<DeviceWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_DeviceWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<ContextWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_ContextWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<EventWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_EventWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<CommandQueueWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_CommandQueueWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<MemoryObjectWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_MemoryObjectWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<ProgramWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_ProgramWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<KernelWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_KernelWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
    template<> inline
    cl_int getInfo (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<SamplerWrapper*>& aValueOut, InfoFuncExtra infoFunc) {
        return getInfoImpl_SamplerWrapper_V (aInstance, aExtra, aName, aValueOut, infoFunc);
    }
}


/** Common base class for OpenCL wrapper classes.
 * This class provides functionality for reference counting based life cycle
 * management as well as weak references for tracking purposes.
 * A common mechanism for accessing OpenCL info parameters, getInfo, provides
 * template based access to (almost) arbitrarily typed values.
 */
class Wrapper {
public:
    /** Weak reference callback function type definition.
     * Any registered weak reference callbacks are called when the instance on
     * which they were registered is destroyed. The aUserData void pointer is
     * stored at the registration time and relayed to callback as an argument.
     */
    typedef void (*WrapperWeakRefCb)(Wrapper* aWrapper, void* aUserData);

    /** Constructor */
    Wrapper ();

    /** Increase the reference count by one.
     * \return OpenCL error code on error, CL_INVALID_VALUE if ref count is 0
     * and CL_SUCCESS on success.
     * */
    cl_int retain ();

    /** Decrease the reference count by one, destroying the instance if
     * the count reaches zero.
     * \return OpenCL error code on error, CL_INVALID_VALUE if ref count is 0
     * and CL_SUCCESS on success.
     * \note It is possible that the return value indicates error but the
     * instance is still destroyed. This happens if the OpenCL release fails
     * for some reason but the internal reference count indicates that
     * the instance should be destroyed.
     */
    cl_int release ();

    /** Returns the current reference count.
     * The return value is the reference count of the wrapper and is equal
     * or lower than the value of the reference counting mechanism possibly
     * implemented by the underlying OpenCL entity.
     */
    size_t refCount () const;

    /** Add a weak reference to the instance.
     * \param aCb Weak reference callback, called when the instance is destroyed.
     * \param aUserData This arbitrary user data is given to the callback as an
     * argument.
     * \return True on success, false if identical weak reference already exists.
     * \see WrapperWeakRefCb
     */
    bool addWeakRef (WrapperWeakRefCb aCb, void* aUserData);

    /** Remove a weak reference from the instance.
     * All weak references on this instance using the given callback are removed.
     * \param aCb The weak reference callback to be removed.
     * \return True on success, false if a matching callback could not be found.
     */
    bool removeWeakRef (WrapperWeakRefCb aCb);

    /** Remove a weak reference from the instance.
     * Only those weak reference on this instance that have a matching user data
     * are removed.
     * \param aCb The weak reference callback to be removed.
     * \param aUserData The user data given during the weak reference
     * registration (addWeakRef).
     * \return True on success, false if a matching callback could not be found.
     * \see addWeakRef
     */
    bool removeWeakRef (WrapperWeakRefCb aCb, void* aUserData);

    /** External data store.
     * This store can be used to bind external data to this instance. The data
     * is stored as key-value pairs, key being a string and value a void
     * pointer.
     * Wrapper does not take ownership of the data.
     */
    std::map<std::string, void*> store;

protected:
    /** Public calls to destructor are prevented.
     * Wrappers and derivative classes should never be directly destroyed
     * using delete due to reference counting mechanism. Always use release ().
     */
    virtual ~Wrapper ();

    /** Internal getInfo wrapper function.
     * This template simply delegates the getInfo call to a specialized template
     * in CLWrapperDetail.
     */
    template<typename T> inline
    cl_int getInfo (int aName, T& aValueOut, InfoFunc infoFunc) const {
        return CLWrapperDetail::getInfo (this, aName, aValueOut, infoFunc);
    }

    /** Internal getInfo with extra data wrapper function.
     * This template simply delegates the getInfo call to a specialized template
     * in CLWrapperDetail.
     */
    template<typename T> inline
    cl_int getInfo (Wrapper const* aExtra, int aName, T& aValueOut,
                    InfoFuncExtra infoFunc) const {
      return CLWrapperDetail::getInfo (this, aExtra, aName, aValueOut, infoFunc);
    }

    /** Increment reference counter.
     * Calls the relevant OpenCL retain if one exists.
     * The subclass must implement this virtual function
     */
    virtual cl_int retainWrapped () const;

    /** Decrement reference counter
     * Calls the relevant OpenCL release if one exists.
     * The subclass must implement this virtual function
     */
    virtual cl_int releaseWrapped () const;

private:
    Wrapper (Wrapper const&);
    Wrapper& operator= (Wrapper const&);
    size_t mRefCnt;
    std::multimap<WrapperWeakRefCb,void*> mWeakRefs;
};


/** ImageFormat
 * \see cl_image_format
 */
struct ImageFormatWrapper {
    /** Constructor */
    ImageFormatWrapper (cl_channel_order aOrder = 0, cl_channel_type aType = 0)
        : channelOrder (aOrder), channelDataType (aType)
    { }

    /** The number of channels and the channel layout.
     * \see cl_image_format
     */
    cl_channel_order channelOrder;

    /** The channel data type.
     * \see cl_image_format
     */
    cl_channel_type channelDataType;
};


/** A specific region in a buffer.
 * \see MemoryObjectWrapper::createSubBuffer
 * \see clCreateSubBuffer
 * \see _cl_buffer_region
 */
struct RegionWrapper {
    /** Constructor */
    RegionWrapper (size_t aOrigin = 0, size_t aSize = 0)
        : origin (aOrigin), size (aSize)
    { }

    /** Origin of the region. */
    size_t origin;
    /** Size of the region. */
    size_t size;
};


#endif // CLWRAPPERCOMMON_H
