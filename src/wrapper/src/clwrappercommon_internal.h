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

/** \file clwrappercommon_internal.h
 * This is an internal header. Do not include from externally available
 * headers in ../include.
 */

#ifndef CLWRAPPERCOMMON_INTERNAL_H
#define CLWRAPPERCOMMON_INTERNAL_H

#include <new> //nothrow

#define LOG_LEVEL_NONE      0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4

#ifndef CL_WRAPPER_LOG_LEVEL_DEFAULT
# define CL_WRAPPER_LOG_LEVEL_DEFAULT 2
#endif

#ifdef CL_WRAPPER_ENABLE_LOG
#include <cstdlib>
#include <cstdio>

extern std::FILE* cl_wrapper_log_file;  // default: stderr or CL_WRAPPER_LOG_TO_FILE
extern int cl_wrapper_log_level;   // default: CL_WRAPPER_LOG_LEVEL_DEFAULT
void cl_wrapper_init_logging ();
bool cl_wrapper_log_check_level (int level);


/**
 * \fn D_PRINT_RAW(...)
 * A variadic macro for printing arbitrary text to log output. In operation
 * only if logging is enabled.
 */
# define D_PRINT_RAW(...) do{cl_wrapper_init_logging(); fprintf(cl_wrapper_log_file, __VA_ARGS__);\
     fprintf(cl_wrapper_log_file, "\n"); fflush(cl_wrapper_log_file);}while(0)

/** \fn D_PRINT(fmt, ...)
 * A variadic macro for printing arbitrary text with source line information
 * to log output. In operation only if logging is enabled.
 * \param fmt Format argument similar to printf.
 * \see printf
 */
# define D_PRINT(fmt, ...) D_PRINT_RAW (" # # # [%s:%-4d] " fmt "\n  ", \
                                        __FILE__,__LINE__,##__VA_ARGS__)

/** \fn D_LOG(lev, fmt, ...)
 * A variadic macro for printing log messages to log output webcl_log_file.
 * The message is sent to output if the value of \c lev is lower than
 * the current logging level.
 * In operation only if logging is enabled.
 * \param lev Logging level assigned to the following message.
 * \param fmt Format argument similar to printf.
 * \see webcl_log_file
 * \see webcl_log_level
 * \see printf
 */
#  define D_LOG(lev, fmt, ...)\
     do{ if(cl_wrapper_log_check_level(lev)){ D_PRINT_RAW(" ##LOG## [%s:%-4d %s] " fmt,\
         __FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__);} }while(0)

#else // CL_WRAPPER_ENABLE_LOG
# ifdef WIN32
// MSVC
#  define D_PRINT_RAW(...) do{}while(0)
#  define D_PRINT(...) do{}while(0)
#  define D_LOG(...) do{}while(0)
# else
#  define D_PRINT_RAW(args...) do{}while(0)
#  define D_PRINT(args...) do{}while(0)
#  define D_LOG(args...) do{}while(0)
# endif
#endif // CL_WRAPPER_ENABLE_LOG


// Function tracking (D_METHOD_START)
#ifdef CL_WRAPPER_TRACK_FUNCTIONS
# define D_METHOD_START D_PRINT_RAW(" ##LOG## [%s:%-4d] ============ Entering function %s ============\n", \
                                    __FILE__, __LINE__,__FUNCTION__)
#else //CL_WRAPPER_TRACK_FUNCTIONS
# define D_METHOD_START do{}while(0)
#endif //CL_WRAPPER_TRACK_FUNCTIONS


// Memory allocation tracking
#ifdef CL_WRAPPER_TRACK_ALLOCS
# define D_TRACK_ALLOC(bytes,type,p) D_PRINT_RAW(" ##LOG## [%s:%-4d %s]  ALLOC  %ld bytes in (" #type "*)%p\n", \
                                    __FILE__, __LINE__,__FUNCTION__, bytes, type, p)
# define D_TRACK_RELEASE(type,p) D_PRINT_RAW(" ##LOG## [%s:%-4d %s]  RELEASE  (" #type "*)%p\n", \
                                    __FILE__, __LINE__,__FUNCTION__, type, p)
#else //CL_WRAPPER_TRACK_ALLOCS
# define D_TRACK_ALLOC do{}while(0)
# define D_TRACK_RELEASE do{}while(0)
#endif //CL_WRAPPER_TRACK_ALLOCS


/** \fn VALIDATE_ARG_WITH_ERROR
 * Argument validation macro.
 * This macro simplifies function argument validation. If \c test
 * fails the function will return with evaluated value of the
 * variadic argument ... as the return value.
 * \param argName Name of the argument being tested (without quotes!)
 * \param test The actual test, e.g. aNumber > 3 .
 * \param statusPtr A pointer to which the error code is stored on failure,
 *  or null value to disable, e.g. an out parameter.
 * \param errNum The error code that is written through statusPtr on failure.
 * \param ... The function return value.
 */
#ifdef WIN32
# define VALIDATE_ARG_WITH_ERROR(argName, test, statusPtr, errNum, ...) do{ \
  if ( !(test)) { \
    if (statusPtr) *statusPtr = errNum; \
    D_LOG (LOG_LEVEL_ERROR, "Invalid argument " #argName ": failed test \"" #test "\"."); \
    return (__VA_ARGS__); \
  } \
}while(0)
#else
# define VALIDATE_ARG_WITH_ERROR(argName, test, statusPtr, errNum, rv...) do{ \
  if ( !(test)) { \
    if (statusPtr) *statusPtr = errNum; \
    D_LOG (LOG_LEVEL_ERROR, "Invalid argument " #argName ": failed test \"" #test "\"."); \
    return (rv); \
  } \
}while(0)
#endif


/** \fn VALIDATE_ARG
 * Argument validation macro.
 * This macro simplifies function argument validation. If \c test
 * fails the function will return with evaluated value of the
 * variadic argument ... as the return value.
 * \param argName Name of the argument being tested (without quotes!)
 * \param test The actual test, e.g. aNumber > 3 .
 * \param statusPtr If non-null, error code CL_INVALID_ARG_VALUE is written
 *  through this pointer.
 * \param ... The function return value.
 */
#ifdef WIN32
# define VALIDATE_ARG(argName, test, statusPtr, ...) \
  VALIDATE_ARG_WITH_ERROR (argName, test, statusPtr, CL_INVALID_ARG_VALUE, __VA_ARGS__)
#else
# define VALIDATE_ARG(argName, test, statusPtr, rv...) \
  VALIDATE_ARG_WITH_ERROR (argName, test, statusPtr, CL_INVALID_ARG_VALUE, rv)
#endif


/** \fn VALIDATE_ARG_POINTER_WITH_ERROR
 * Pointer argument validation macro.
 * This macro simplifies function argument validation. If \c ptr
 * equals to null the function will return with evaluated value of the
 * variadic argument ... as the return value.
 * \param ptr The argument being tested.
 * \param statusPtr A pointer to which the error code is stored on failure,
 *  or null value to disable, e.g. an out parameter.
 * \param errNum The error code that is written through statusPtr on failure.
 * \param ... The function return value.
 */
#ifdef WIN32
# define VALIDATE_ARG_POINTER_WITH_ERROR(ptr, statusPtr, errNum, ...) do{ \
  if (ptr == 0) { \
    void* tmp = statusPtr; /* Avoid gcc warning.. */ \
    if (tmp) *statusPtr = errNum; \
    D_LOG (LOG_LEVEL_ERROR, "Invalid argument " #ptr ": null pointer."); \
    return (__VA_ARGS__); \
  } \
}while(0)
#else
# define VALIDATE_ARG_POINTER_WITH_ERROR(ptr, statusPtr, errNum, rv...) do{ \
  if (ptr == 0) { \
    void* tmp = statusPtr; /* Avoid gcc warning.. */ \
    if (tmp) *statusPtr = errNum; \
    D_LOG (LOG_LEVEL_ERROR, "Invalid argument " #ptr ": null pointer."); \
    return (rv); \
  } \
}while(0)
#endif


/** \fn VALIDATE_ARG_POINTER
 * Pointer argument validation macro.
 * This macro simplifies function argument validation. If \c ptr
 * equals to null the function will return with evaluated value of the
 * variadic argument ... as the return value.
 * \param ptr The argument being tested.
 * \param statusPtr If non-null, error code CL_INVALID_ARG_VALUE is written
 *  through this pointer.
 * \param ... The function return value.
 */
#ifdef WIN32
# define VALIDATE_ARG_POINTER(ptr, statusPtr, ...) \
  VALIDATE_ARG_POINTER_WITH_ERROR (ptr, statusPtr, CL_INVALID_ARG_VALUE, __VA_ARGS__)
#else
# define VALIDATE_ARG_POINTER(ptr, statusPtr, rv...) \
  VALIDATE_ARG_POINTER_WITH_ERROR (ptr, statusPtr, CL_INVALID_ARG_VALUE, rv)
#endif


#endif // CLWRAPPERCOMMON_INTERNAL_H
