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
 * Enumeration of supported types for the info parameter getters (getInfo)
 * and setKernelArg.
 */

#ifndef _CLWRAPPERTYPES_H_
#define _CLWRAPPERTYPES_H_

namespace types {
enum CLType {
    UNKNOWN,

    BYTE,
    CHAR,
    UCHAR,
    SHORT,
    USHORT,

    // Basic types
    INT,                        // cl_int
    UINT,                       // cl_uint
    LONG,                       // cl_long
    ULONG,                      // cl_ulong
    BOOL,                       // cl_bool = cl_uint                    //10
    SIZE_T,                     // size_t
    HALF,                       // cl_half
    FLOAT,                      // cl_float
    DOUBLE,                     // cl_double

    // String types
    STRING,                     // char*

    // Class types
    PLATFORM,                   // cl_platform_id
    DEVICE,                     // cl_device_id
    CONTEXT,                    // cl_context
    COMMAND_QUEUE,              // cl_command_queue
    MEMORY_OBJECT,              // cl_mem                               //20
    PROGRAM,                    // cl_program
    KERNEL,                     // cl_kernel
    EVENT,                      // cl_event
    SAMPLER,                    // cl_sampler

    IMAGE_FORMAT,

    // Special types
    ADRESSING_MODE,             // cl_addressing_mode
    BUILD_STATUS,               // cl_build_status
    CHANNEL_ORDER,              // cl_channel_order
    CHANNEL_TYPE,               // cl_channel_type
    COMMAND_QUEUE_PROPERTIES,   // cl_command_queue_properties          //30
    COMMAND_TYPE,               // cl_command_type
    CONTEXT_PROPERTIES,         // cl_context_properties
    // cl_d3d10_device_set_khr, cl_d3d10_device_source_khr removed
    DEVICE_EXEC_CAPABILITIES,   // cl_device_exec_capabilities
    DEVICE_FP_CONFIG,           // cl_device_fp_config
    DEVICE_LOCAL_MEM_TYPE,      // cl_device_local_mem_type
    DEVICE_MEM_CACHE_TYPE,      // cl_device_mem_cache_type
    DEVICE_TYPE,                // cl_device_type
    FILTER_MODE,                // cl_filter_mode
    GL_OBJECT_TYPE,             // cl_gl_object_type
    MAP_FLAGS,                  // cl_map_flags                         //40
    MEM_FENCE_FLAGS,            // cl_mem_fence_flags
    MEM_FLAGS,                  // cl_mem_flags
    MEM_OBJECT_TYPE,            // cl_mem_object_type

    // Vector types
    BYTE_V,
    CHAR_V,
    UCHAR_V,
    SHORT_V,
    USHORT_V,
    INT_V,                      // cl_int*
    UINT_V,                     // cl_uint*                             //50
    LONG_V,                     // cl_long*
    ULONG_V,                    // cl_ulong*
    BOOL_V,                     // cl_bool*
    SIZE_T_V,                   // size_t*
    HALF_V,
    FLOAT_V,
    DOUBLE_V,
    STRING_V,                   // char**

    PLATFORM_V,
    DEVICE_V,                                                           //60
    CONTEXT_V,
    COMMAND_QUEUE_V,
    MEMORY_OBJECT_V,
    PROGRAM_V,
    KERNEL_V,
    EVENT_V,
    SAMPLER_V,

    LAST
};
}

#endif // _CLWRAPPERTYPES_H_
