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

/** \file clwrappercommon.cpp
 * Implementations of functions and classes defined in clwrappercommon.h
 * and clwrappercommon_internal.h .
 */

// Prevent MSVC warnings about freopen
#define _CRT_SECURE_NO_WARNINGS

// NOTE: clwrappercommon_internal.h MUST be included before clwrappercommon.h
// because there are some dependencies that matter for internal use.
#include "clwrappercommon_internal.h"
#include "clwrappercommon.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <typeinfo>



//============================================================================
// from clwrappercommon.h:

#include "platformwrapper.h"
#include "devicewrapper.h"
#include "contextwrapper.h"
#include "memoryobjectwrapper.h"
#include "programwrapper.h"
#include "kernelwrapper.h"
#include "commandqueuewrapper.h"
#include "eventwrapper.h"
#include "samplerwrapper.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::multimap;
using std::make_pair;


#define LOGGER_BUF_SIZE 1024

void CLWrapperDetail::logger (char const* file, unsigned int line,
                              char const* function, int level, char const* msg, ...) {
#ifdef CL_WRAPPER_ENABLE_LOG
    char* s = (char*)malloc (sizeof(char) * LOGGER_BUF_SIZE);
    if (s) {
        if (cl_wrapper_log_check_level(level)) {
            va_list ap;
            va_start (ap, msg);
            vsnprintf (s, LOGGER_BUF_SIZE, msg, ap);
            va_end (ap);
            D_PRINT_RAW(" ##LOG## [%s:%d %s] %s", file, line, function, s);
        }
        free (s);
    }
#else // CL_WRAPPER_ENABLE_LOG
    (void)file;
    (void)line;
    (void)function;
    (void)level;
    (void)msg;
#endif // CL_WRAPPER_ENABLE_LOG
}



cl_int CLWrapperDetail::getInfoImpl_string (Wrapper const* aInstance, int aName, std::string& aValueOut, InfoFunc infoFunc) {
    cl_int err = CL_SUCCESS;
    char* buf = 0;
    size_t sze = 0;
    if (!infoFunc) {
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
        return CL_INVALID_VALUE;
    }
    err = infoFunc (aInstance, aName, 0, 0, &sze);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf = (char*)malloc (sizeof (char) * (sze + 1));
    if (!buf) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    err = infoFunc (aInstance, aName, sze, (void*)buf, 0);
    if (err != CL_SUCCESS) {
        free (buf);
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf[sze] = '\0'; // Just to be safe..
    aValueOut = buf;
    free (buf);
    return err;
}

cl_int CLWrapperDetail::getInfoImpl_string_V (Wrapper const* aInstance, int aName, std::vector<std::string>& aValueOut, InfoFunc infoFunc) {
    char** buf = 0;
    size_t sze = 0;
    cl_int err = CL_SUCCESS;
    if (!infoFunc) {
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
        return CL_INVALID_VALUE;
    }
    err = infoFunc (aInstance, aName, 0, 0, &sze);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf = (char**)malloc (sze);
    if (!buf) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    err = infoFunc (aInstance, aName, sze, (void*)buf, 0);
    if (err != CL_SUCCESS) {
        free (buf);
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    aValueOut.clear ();
    size_t num = sze / sizeof (char*);
    aValueOut.reserve (num);
    for (size_t i = 0; i < num; ++i)
        aValueOut.push_back (string (buf[i]));
    free (buf);
    return err;
}


#define IMPL_GET_INFO_FOR_CLASS(className,clType) \
cl_int CLWrapperDetail::getInfoImpl_##className (Wrapper const* aInstance, int aName, className*& aValueOut, InfoFunc infoFunc) { \
    cl_int err = CL_SUCCESS; \
    size_t sze = 0; \
    clType clHandle = 0; \
    if (!infoFunc) { \
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null)."); \
        return CL_INVALID_VALUE; \
    } \
    err = infoFunc (aInstance, aName, sizeof(clType), (void*)&clHandle, &sze); \
    if (err != CL_SUCCESS) { \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    if (sze != sizeof(clType)) { \
        D_LOG (LOG_LEVEL_ERROR, \
             "getInfo returned a value of unexpected size %u, expected (%u bytes)", \
             sze, sizeof(clType)); \
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value."); \
        return CL_INVALID_VALUE; \
    } \
    aValueOut = className::getNewOrExisting (clHandle); \
    return err; \
}

IMPL_GET_INFO_FOR_CLASS (PlatformWrapper, cl_platform_id);
IMPL_GET_INFO_FOR_CLASS (DeviceWrapper, cl_device_id);
IMPL_GET_INFO_FOR_CLASS (ContextWrapper, cl_context);
IMPL_GET_INFO_FOR_CLASS (EventWrapper, cl_event);
IMPL_GET_INFO_FOR_CLASS (CommandQueueWrapper, cl_command_queue);
IMPL_GET_INFO_FOR_CLASS (MemoryObjectWrapper, cl_mem);
IMPL_GET_INFO_FOR_CLASS (ProgramWrapper, cl_program);
IMPL_GET_INFO_FOR_CLASS (KernelWrapper, cl_kernel);
IMPL_GET_INFO_FOR_CLASS (SamplerWrapper, cl_sampler);


#define IMPL_GET_INFO_FOR_CLASS_VECTOR(className,clType) \
cl_int CLWrapperDetail::getInfoImpl_##className##_V (Wrapper const* aInstance, int aName, std::vector<className*>& aValueOut, InfoFunc infoFunc) { \
    size_t sze = 0; \
    cl_int err = CL_SUCCESS; \
    if (!infoFunc) { \
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null)."); \
        return CL_INVALID_VALUE; \
    } \
    err = infoFunc (aInstance, aName, 0, 0, &sze); \
    if (err != CL_SUCCESS) { \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    clType* buf = (clType*)malloc (sze); /*(sizeof (clType) * sze);*/ \
    if (!buf) { \
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed."); \
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value."); \
        return CL_OUT_OF_HOST_MEMORY; \
    } \
    err = infoFunc (aInstance, aName, sze, (void*)buf, 0); \
    if (err != CL_SUCCESS) { \
        free (buf); \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    aValueOut.clear (); \
    size_t num = sze / sizeof (clType); \
    aValueOut.reserve (num); \
    for (size_t i = 0; i < num; ++i) \
        aValueOut.push_back (className::getNewOrExisting (buf[i])); \
    free (buf); \
    return err; \
}

IMPL_GET_INFO_FOR_CLASS_VECTOR (PlatformWrapper, cl_platform_id);
IMPL_GET_INFO_FOR_CLASS_VECTOR (DeviceWrapper, cl_device_id);
IMPL_GET_INFO_FOR_CLASS_VECTOR (ContextWrapper, cl_context);
IMPL_GET_INFO_FOR_CLASS_VECTOR (EventWrapper, cl_event);
IMPL_GET_INFO_FOR_CLASS_VECTOR (CommandQueueWrapper, cl_command_queue);
IMPL_GET_INFO_FOR_CLASS_VECTOR (MemoryObjectWrapper, cl_mem);
IMPL_GET_INFO_FOR_CLASS_VECTOR (ProgramWrapper, cl_program);
IMPL_GET_INFO_FOR_CLASS_VECTOR (KernelWrapper, cl_kernel);
IMPL_GET_INFO_FOR_CLASS_VECTOR (SamplerWrapper, cl_sampler);


cl_int CLWrapperDetail::getInfoImpl_string (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::string& aValueOut, InfoFuncExtra infoFunc) {
    cl_int err = CL_SUCCESS;
    char* buf = 0;
    size_t sze = 0;
    if (!infoFunc) {
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
        return CL_INVALID_VALUE;
    }
    err = infoFunc (aInstance, aExtra, aName, 0, 0, &sze);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf = (char*)malloc (sizeof (char) * (sze + 1));
    if (!buf) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    err = infoFunc (aInstance, aExtra, aName, sze, (void*)buf, 0);
    if (err != CL_SUCCESS) {
        free (buf);
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf[sze] = '\0'; // Just to be safe..
    aValueOut = buf;
    free (buf);
    return err;
}

cl_int CLWrapperDetail::getInfoImpl_string_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<std::string>& aValueOut, InfoFuncExtra infoFunc) {
    char** buf = 0;
    size_t sze = 0;
    if (!infoFunc) {
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null).");
        return CL_INVALID_VALUE;
    }
    cl_int err = infoFunc (aInstance, aExtra, aName, 0, 0, &sze);
    if (err != CL_SUCCESS) {
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    buf = (char**)malloc (sze);
    if (!buf) {
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed.");
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
        return CL_OUT_OF_HOST_MEMORY;
    }
    err = infoFunc (aInstance, aExtra, aName, sze, (void*)buf, 0);
    if (err != CL_SUCCESS) {
        free (buf);
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err);
        return err;
    }
    aValueOut.clear ();
    size_t num = sze / sizeof (char*);
    aValueOut.reserve (num);
    for (size_t i = 0; i < num; ++i)
        aValueOut.push_back (string (buf[i]));
    free (buf);
    return err;
}


#define IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS(className,clType) \
cl_int CLWrapperDetail::getInfoImpl_##className (Wrapper const* aInstance, Wrapper const* aExtra, int aName, className*& aValueOut, InfoFuncExtra infoFunc) { \
    cl_int err = CL_SUCCESS; \
    size_t sze = 0; \
    clType clHandle = 0; \
    if (!infoFunc) { \
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null)."); \
        return CL_INVALID_VALUE; \
    } \
    err = infoFunc (aInstance, aExtra, aName, sizeof(clType), (void*)&clHandle, &sze); \
    if (err != CL_SUCCESS) { \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    if (sze != sizeof(clType)) { \
        D_LOG (LOG_LEVEL_ERROR, \
             "getInfo returned a value of unexpected size %u, expected (%u bytes)", \
             sze, sizeof(clType)); \
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value."); \
        return CL_INVALID_VALUE; \
    } \
    aValueOut = className::getNewOrExisting (clHandle); \
    return err; \
}

IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (PlatformWrapper, cl_platform_id);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (DeviceWrapper, cl_device_id);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (ContextWrapper, cl_context);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (EventWrapper, cl_event);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (CommandQueueWrapper, cl_command_queue);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (MemoryObjectWrapper, cl_mem);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (ProgramWrapper, cl_program);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (KernelWrapper, cl_kernel);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS (SamplerWrapper, cl_sampler);


#define IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR(className,clType) \
cl_int CLWrapperDetail::getInfoImpl_##className##_V (Wrapper const* aInstance, Wrapper const* aExtra, int aName, std::vector<className*>& aValueOut, InfoFuncExtra infoFunc) { \
    cl_int err = CL_SUCCESS; \
    size_t sze = 0; \
    if (!infoFunc) { \
        D_LOG (LOG_LEVEL_ERROR, "Invalid infoFunc argument (null)."); \
        return CL_INVALID_VALUE; \
    } \
    err = infoFunc (aInstance, aExtra, aName, 0, 0, &sze); \
    if (err != CL_SUCCESS) { \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    clType* buf = (clType*)malloc (sze); /*(sizeof (clType) * sze);*/ \
    if (!buf) { \
        D_LOG (LOG_LEVEL_ERROR, "Memory allocation failed."); \
        D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value."); \
        return CL_OUT_OF_HOST_MEMORY; \
    } \
    err = infoFunc (aInstance, aExtra, aName, sze, (void*)buf, 0); \
    if (err != CL_SUCCESS) { \
        free (buf); \
        D_LOG (LOG_LEVEL_ERROR, "getInfo for %d failed. (error %d)", aName, err); \
        return err; \
    } \
    aValueOut.clear (); \
    size_t num = sze / sizeof (clType); \
    aValueOut.reserve (num); \
    for (size_t i = 0; i < num; ++i) \
        aValueOut.push_back (className::getNewOrExisting (buf[i])); \
    free (buf); \
    return err; \
}

IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (PlatformWrapper, cl_platform_id);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (DeviceWrapper, cl_device_id);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (ContextWrapper, cl_context);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (EventWrapper, cl_event);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (CommandQueueWrapper, cl_command_queue);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (MemoryObjectWrapper, cl_mem);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (ProgramWrapper, cl_program);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (KernelWrapper, cl_kernel);
IMPL_GET_INFO_WITH_EXTRA_FOR_CLASS_VECTOR (SamplerWrapper, cl_sampler);



Wrapper::Wrapper ()
    : store (),
      mRefCnt (1),
      mWeakRefs ()
{
}


Wrapper::~Wrapper () {
    if (mRefCnt > 0)
        releaseWrapped ();

    multimap<WrapperWeakRefCb,void*>::iterator i = mWeakRefs.begin ();
    while (i != mWeakRefs.end ()) {
      if (i->first) {
        (i->first)(this, i->second);
      } else {
        D_LOG (LOG_LEVEL_WARNING, "Invalid null weak ref callback with user data %p.", i->second);
      }
    }
}


cl_int Wrapper::retain () {
    if (mRefCnt > 0) {
        ++mRefCnt;
        return retainWrapped();
    } else {
        D_LOG (LOG_LEVEL_ERROR, "Invalid addRef on %p when refcount is 0.", this);
        return CL_INVALID_VALUE;
    }
}


cl_int Wrapper::release () {
    if (mRefCnt <= 0) {
        D_LOG (LOG_LEVEL_ERROR, "Invalid release on %p when refcount already 0.", this);
        return CL_INVALID_VALUE;
    } else {
        cl_int err = releaseWrapped();
        if (--mRefCnt == 0) {
            delete this;
        }
        return err;
    }
}


size_t Wrapper::refCount () const {
    return mRefCnt;
}


bool Wrapper::addWeakRef (WrapperWeakRefCb aCb, void* aUserData) {
    D_METHOD_START;
    D_LOG (LOG_LEVEL_DEBUG, "    callback: %p, userdata: %p", aCb, aUserData);
    multimap<WrapperWeakRefCb,void*>::const_iterator i = mWeakRefs.find (aCb);
    if (i != mWeakRefs.end ()) {
        if (i->second == aUserData) {
            // Identical weak ref exists already -> ignored.
            D_LOG (LOG_LEVEL_DEBUG, "    ignored!");
            return false;
        }
        D_LOG (LOG_LEVEL_DEBUG, "    callback already exists but userData doesn't match!");
    }
    // Add weak ref
    mWeakRefs.insert (make_pair (aCb, aUserData));
    return true;
}


bool Wrapper::removeWeakRef (WrapperWeakRefCb aCb) {
    D_METHOD_START;
    D_LOG (LOG_LEVEL_DEBUG, "    callback: %p", aCb);
    multimap<WrapperWeakRefCb,void*>::iterator i = mWeakRefs.find (aCb);
    if (i != mWeakRefs.end ()) {
        mWeakRefs.erase (i);
        return true;
    }
    D_LOG (LOG_LEVEL_WARNING, "weak ref callback %p not found!", aCb);
    return false;
}


bool Wrapper::removeWeakRef (WrapperWeakRefCb aCb, void* aUserData) {
    D_METHOD_START;
    D_LOG (LOG_LEVEL_DEBUG, "    callback: %p, userdata: %p", aCb, aUserData);
    multimap<WrapperWeakRefCb,void*>::iterator i = mWeakRefs.find (aCb);
    if ((i != mWeakRefs.end ()) && (i->second == aUserData)) {
        mWeakRefs.erase (i);
        return true;
    }
    D_LOG (LOG_LEVEL_WARNING, "weak ref callback %p with data %p not found!", aCb, aUserData);
    return false;
}


cl_int Wrapper::retainWrapped () const {
    D_LOG (LOG_LEVEL_WARNING,
           "Unexpected call to Wrapper::retainWrapped: derived class doesn't implement retain!");
    D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
    return CL_INVALID_OPERATION;
}


cl_int Wrapper::releaseWrapped () const {
    D_LOG (LOG_LEVEL_WARNING,
           "Unexpected call to Wrapper::releaseWrapped: derived class doesn't implement release!");
    D_LOG (LOG_LEVEL_WARNING, "Returning synthetic error value.");
    return CL_INVALID_OPERATION;
}



//============================================================================
// from clwrappercommon_internal.h:

FILE* cl_wrapper_log_file = stderr;
int cl_wrapper_log_level = CL_WRAPPER_LOG_LEVEL_DEFAULT;

#ifdef CL_WRAPPER_LOG_TO_FILE
#include <ctime>
# ifndef WIN32
# include <cstring>
# include <cerrno>
# endif
#endif


#ifdef WIN32
#include "windows.h"
void cl_wrapper_init_logging () {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
 #ifdef CL_WRAPPER_LOG_TO_FILE
    if ((cl_wrapper_log_file = fopen (CL_WRAPPER_LOG_TO_FILE, "a")) == NULL) {
      cl_wrapper_log_file = stderr;
      D_PRINT ("Failed to open log file \"%s\".", CL_WRAPPER_LOG_TO_FILE);
    } else {
      time_t t = time(NULL);
      struct tm *tmp = localtime(&t);
      char timeStr[30] = {0};
      strftime (timeStr, 30, "%Y-%m-%d %M:%H:%S %Z", tmp);
      D_PRINT_RAW (" =============================================================================\n"
                   "  CL Wrapper log file opened %s.\n"
                   " =============================================================================\n",
                   timeStr);
    }
 #else //CL_WRAPPER_LOG_TO_FILE
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
 #endif //CL_WRAPPER_LOG_TO_FILE
  }
}
#else //WIN32
void cl_wrapper_init_logging () {
  static bool initialized = false;
  if (!initialized) {
    char* s = std::getenv("D_LOG_LEVEL");
    if (s) {
      cl_wrapper_log_level = std::atoi(s);
    }
    initialized = true;
 #ifdef CL_WRAPPER_LOG_TO_FILE
    if ((cl_wrapper_log_file = std::fopen (CL_WRAPPER_LOG_TO_FILE, "a")) == NULL) {
      cl_wrapper_log_file = stderr;
      D_PRINT ("Failed to open log file \"%s\": %s", CL_WRAPPER_LOG_TO_FILE, strerror(errno));
    } else {
      time_t t = time(NULL);
      struct tm *tmp = localtime(&t);
      char timeStr[30] = {0};
      strftime (timeStr, 30, "%Y-%m-%d %M:%H:%S %Z", tmp);
      D_PRINT_RAW (" =============================================================================\n"
                   "  CL Wrapper log file opened %s.\n"
                   " =============================================================================\n",
                   timeStr);
    }
 #endif //CL_WRAPPER_LOG_TO_FILE
  }
}
#endif //WIN32


bool cl_wrapper_log_check_level (int level) {
  return level <= cl_wrapper_log_level;
}

