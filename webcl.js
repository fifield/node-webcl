
var cl = require("_webcl");

var webcl = new cl.WebCL();

exports.WebCL = webcl;

exports.WebCLCommandQueue = cl.WebCLCommandQueue;
exports.WebCLContext = cl.WebCLContext;
exports.WebCLDevice= cl.WebCLDevice;
exports.WebCLEvent = cl.WebCLEvent;
exports.WebCLKernel = cl.WebCLKernel;
exports.WebCLMemoryObject = cl.WebCLMemoryObject;
exports.WebCLPlatform = cl.WebCLPlatform;
exports.WebCLProgram = cl.WebCLProgram;
exports.WebCLSampler = cl.WebCLSampler;

//
// WebCL Interface
//

// Functions

//  WebCLPlatform[] getPlatforms();
exports.getPlatforms = function() { 
    return webcl.getPlatforms();
};

//  WebCLContext? createContext(optional WebCLContextProperties properties);
exports.createContext = function(a,b) { 
    return webcl.createContext(a,b);
};

//  not in spec
exports.createContextFromType = function(a,b) { 
    return webcl.createContextFromType(a,b);
};

//  DOMString[] getSupportedExtensions();
exports.getSupportedExtensions = function() {
    return "";
};

//  void waitForEvents(WebCLEvent[] eventWaitList);
exports.waitForEvents = function() { 
    return webcl.waitForEvents(arguments);
};

//  void unloadCompiler();
exports.unloadCompiler = function() { 
    return webcl.unloadCompiler();
};

// Enums

  /* Error Codes */
exports.SUCCESS                                  =0;
exports.DEVICE_NOT_FOUND                         =-1;
exports.DEVICE_NOT_AVAILABLE                     =-2;
exports.COMPILER_NOT_AVAILABLE                   =-3;
exports.MEM_OBJECT_ALLOCATION_FAILURE            =-4;
exports.OUT_OF_RESOURCES                         =-5;
exports.OUT_OF_HOST_MEMORY                       =-6;
exports.PROFILING_INFO_NOT_AVAILABLE             =-7;
exports.MEM_COPY_OVERLAP                         =-8;
exports.IMAGE_FORMAT_MISMATCH                    =-9;
exports.IMAGE_FORMAT_NOT_SUPPORTED               =-10;
exports.BUILD_PROGRAM_FAILURE                    =-11;
exports.MAP_FAILURE                              =-12;
exports.MISALIGNED_SUB_BUFFER_OFFSET             =-13;
exports.EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST =-14;
exports.INVALID_VALUE                            =-30;
exports.INVALID_DEVICE_TYPE                      =-31;
exports.INVALID_PLATFORM                         =-32;
exports.INVALID_DEVICE                           =-33;
exports.INVALID_CONTEXT                          =-34;
exports.INVALID_QUEUE_PROPERTIES                 =-35;
exports.INVALID_COMMAND_QUEUE                    =-36;
exports.INVALID_HOST_PTR                         =-37;
exports.INVALID_MEM_OBJECT                       =-38;
exports.INVALID_IMAGE_FORMAT_DESCRIPTOR          =-39;
exports.INVALID_IMAGE_SIZE                       =-40;
exports.INVALID_SAMPLER                          =-41;
exports.INVALID_BINARY                           =-42;
exports.INVALID_BUILD_OPTIONS                    =-43;
exports.INVALID_PROGRAM                          =-44;
exports.INVALID_PROGRAM_EXECUTABLE               =-45;
exports.INVALID_KERNEL_NAME                      =-46;
exports.INVALID_KERNEL_DEFINITION                =-47;
exports.INVALID_KERNEL                           =-48;
exports.INVALID_ARG_INDEX                        =-49;
exports.INVALID_ARG_VALUE                        =-50;
exports.INVALID_ARG_SIZE                         =-51;
exports.INVALID_KERNEL_ARGS                      =-52;
exports.INVALID_WORK_DIMENSION                   =-53;
exports.INVALID_WORK_GROUP_SIZE                  =-54;
exports.INVALID_WORK_ITEM_SIZE                   =-55;
exports.INVALID_GLOBAL_OFFSET                    =-56;
exports.INVALID_EVENT_WAIT_LIST                  =-57;
exports.INVALID_EVENT                            =-58;
exports.INVALID_OPERATION                        =-59;
exports.INVALID_GL_OBJECT                        =-60;
exports.INVALID_BUFFER_SIZE                      =-61;
exports.INVALID_MIP_LEVEL                        =-62;
exports.INVALID_GLOBAL_WORK_SIZE                 =-63;
exports.INVALID_PROPERTY                         =-64;

  /* OpenCL Version */
exports.VERSION_1_0                              =1;
exports.VERSION_1_1                              =1;

  /* cl_bool */
exports.FALSE                                    =0;
exports.TRUE                                     =1;

  /* cl_platform_info */
exports.PLATFORM_PROFILE                         =0x0900;
exports.PLATFORM_VERSION                         =0x0901;
exports.PLATFORM_NAME                            =0x0902;
exports.PLATFORM_VENDOR                          =0x0903;
exports.PLATFORM_EXTENSIONS                      =0x0904;

  /* cl_device_type - bitfield */
exports.DEVICE_TYPE_DEFAULT                      =(1 << 0);
exports.DEVICE_TYPE_CPU                          =(1 << 1);
exports.DEVICE_TYPE_GPU                          =(1 << 2);
exports.DEVICE_TYPE_ACCELERATOR                  =(1 << 3);
exports.DEVICE_TYPE_ALL                          =0xFFFFFFFF;

  /* cl_device_info */
exports.DEVICE_TYPE                              =0x1000;
exports.DEVICE_VENDOR_ID                         =0x1001;
exports.DEVICE_MAX_COMPUTE_UNITS                 =0x1002;
exports.DEVICE_MAX_WORK_ITEM_DIMENSIONS          =0x1003;
exports.DEVICE_MAX_WORK_GROUP_SIZE               =0x1004;
exports.DEVICE_MAX_WORK_ITEM_SIZES               =0x1005;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_CHAR       =0x1006;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_SHORT      =0x1007;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_INT        =0x1008;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_LONG       =0x1009;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT      =0x100A;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE     =0x100B;
exports.DEVICE_MAX_CLOCK_FREQUENCY               =0x100C;
exports.DEVICE_ADDRESS_BITS                      =0x100D;
exports.DEVICE_MAX_READ_IMAGE_ARGS               =0x100E;
exports.DEVICE_MAX_WRITE_IMAGE_ARGS              =0x100F;
exports.DEVICE_MAX_MEM_ALLOC_SIZE                =0x1010;
exports.DEVICE_IMAGE2D_MAX_WIDTH                 =0x1011;
exports.DEVICE_IMAGE2D_MAX_HEIGHT                =0x1012;
exports.DEVICE_IMAGE_SUPPORT                     =0x1016;
exports.DEVICE_MAX_PARAMETER_SIZE                =0x1017;
exports.DEVICE_MAX_SAMPLERS                      =0x1018;
exports.DEVICE_MEM_BASE_ADDR_ALIGN               =0x1019;
exports.DEVICE_MIN_DATA_TYPE_ALIGN_SIZE          =0x101A;
exports.DEVICE_SINGLE_FP_CONFIG                  =0x101B;
exports.DEVICE_GLOBAL_MEM_CACHE_TYPE             =0x101C;
exports.DEVICE_GLOBAL_MEM_CACHELINE_SIZE         =0x101D;
exports.DEVICE_GLOBAL_MEM_CACHE_SIZE             =0x101E;
exports.DEVICE_GLOBAL_MEM_SIZE                   =0x101F;
exports.DEVICE_MAX_CONSTANT_BUFFER_SIZE          =0x1020;
exports.DEVICE_MAX_CONSTANT_ARGS                 =0x1021;
exports.DEVICE_LOCAL_MEM_TYPE                    =0x1022;
exports.DEVICE_LOCAL_MEM_SIZE                    =0x1023;
exports.DEVICE_ERROR_CORRECTION_SUPPORT          =0x1024;
exports.DEVICE_PROFILING_TIMER_RESOLUTION        =0x1025;
exports.DEVICE_ENDIAN_LITTLE                     =0x1026;
exports.DEVICE_AVAILABLE                         =0x1027;
exports.DEVICE_COMPILER_AVAILABLE                =0x1028;
exports.DEVICE_EXECUTION_CAPABILITIES            =0x1029;
exports.DEVICE_QUEUE_PROPERTIES                  =0x102A;
exports.DEVICE_NAME                              =0x102B;
exports.DEVICE_VENDOR                            =0x102C;
exports.DRIVER_VERSION                           =0x102D;
exports.DEVICE_PROFILE                           =0x102E;
exports.DEVICE_VERSION                           =0x102F;
exports.DEVICE_EXTENSIONS                        =0x1030;
exports.DEVICE_PLATFORM                          =0x1031;
exports.DEVICE_DOUBLE_FP_CONFIG                  =0x1032;
exports.DEVICE_HALF_FP_CONFIG                    =0x1033;
exports.DEVICE_PREFERRED_VECTOR_WIDTH_HALF       =0x1034;
exports.DEVICE_HOST_UNIFIED_MEMORY               =0x1035;
exports.DEVICE_NATIVE_VECTOR_WIDTH_CHAR          =0x1036;
exports.DEVICE_NATIVE_VECTOR_WIDTH_SHORT         =0x1037;
exports.DEVICE_NATIVE_VECTOR_WIDTH_INT           =0x1038;
exports.DEVICE_NATIVE_VECTOR_WIDTH_LONG          =0x1039;
exports.DEVICE_NATIVE_VECTOR_WIDTH_FLOAT         =0x103A;
exports.DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE        =0x103B;
exports.DEVICE_NATIVE_VECTOR_WIDTH_HALF          =0x103C;
exports.DEVICE_OPENCL_C_VERSION                  =0x103D;

  /* cl_device_fp_config - bitfield */
exports.FP_DENORM                                =(1 << 0);
exports.FP_INF_NAN                               =(1 << 1);
exports.FP_ROUND_TO_NEAREST                      =(1 << 2);
exports.FP_ROUND_TO_ZERO                         =(1 << 3);
exports.FP_ROUND_TO_INF                          =(1 << 4);
exports.FP_FMA                                   =(1 << 5);
exports.FP_SOFT_FLOAT                            =(1 << 6);

  /* cl_device_mem_cache_type */
exports.NONE                                     =0x0;
exports.READ_ONLY_CACHE                          =0x1;
exports.READ_WRITE_CACHE                         =0x2;

  /* cl_device_local_mem_type */
exports.LOCAL                                    =0x1;
exports.GLOBAL                                   =0x2;

  /* cl_device_exec_capabilities - bitfield */
exports.EXEC_KERNEL                              =(1 << 0);
exports.EXEC_NATIVE_KERNEL                       =(1 << 1);

  /* cl_command_queue_properties - bitfield */
exports.QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE      =(1 << 0);
exports.QUEUE_PROFILING_ENABLE                   =(1 << 1);

  /* cl_context_info  */
exports.CONTEXT_REFERENCE_COUNT                  =0x1080;
exports.CONTEXT_DEVICES                          =0x1081;
exports.CONTEXT_PROPERTIES                       =0x1082;
exports.CONTEXT_NUM_DEVICES                      =0x1083;

  /* cl_context_info + cl_context_properties */
exports.CONTEXT_PLATFORM                         =0x1084;

  /* cl_command_queue_info */
exports.QUEUE_CONTEXT                            =0x1090;
exports.QUEUE_DEVICE                             =0x1091;
exports.QUEUE_REFERENCE_COUNT                    =0x1092;
exports.QUEUE_PROPERTIES                         =0x1093;

  /* cl_mem_flags - bitfield */
exports.MEM_READ_WRITE                           =(1 << 0);
exports.MEM_WRITE_ONLY                           =(1 << 1);
exports.MEM_READ_ONLY                            =(1 << 2);

  /* cl_channel_order */
exports.R                                        =0x10B0;
exports.A                                        =0x10B1;
exports.RG                                       =0x10B2;
exports.RA                                       =0x10B3;
exports.RGB                                      =0x10B4;
exports.RGBA                                     =0x10B5;
exports.BGRA                                     =0x10B6;
exports.ARGB                                     =0x10B7;
exports.INTENSITY                                =0x10B8;
exports.LUMINANCE                                =0x10B9;
exports.Rx                                       =0x10BA;
exports.RGx                                      =0x10BB;
exports.RGBx                                     =0x10BC;

  /* cl_channel_type */
exports.SNORM_INT8                               =0x10D0;
exports.SNORM_INT16                              =0x10D1;
exports.UNORM_INT8                               =0x10D2;
exports.UNORM_INT16                              =0x10D3;
exports.UNORM_SHORT_565                          =0x10D4;
exports.UNORM_SHORT_555                          =0x10D5;
exports.UNORM_INT_101010                         =0x10D6;
exports.SIGNED_INT8                              =0x10D7;
exports.SIGNED_INT16                             =0x10D8;
exports.SIGNED_INT32                             =0x10D9;
exports.UNSIGNED_INT8                            =0x10DA;
exports.UNSIGNED_INT16                           =0x10DB;
exports.UNSIGNED_INT32                           =0x10DC;
exports.HALF_FLOAT                               =0x10DD;
exports.FLOAT                                    =0x10DE;

  /* cl_mem_object_type */
exports.MEM_OBJECT_BUFFER                        =0x10F0;
exports.MEM_OBJECT_IMAGE2D                       =0x10F1;

  /* cl_mem_info */
exports.MEM_TYPE                                 =0x1100;
exports.MEM_FLAGS                                =0x1101;
exports.MEM_SIZE                                 =0x1102;
exports.MEM_HOST_PTR                             =0x1103;
exports.MEM_MAP_COUNT                            =0x1104;
exports.MEM_REFERENCE_COUNT                      =0x1105;
exports.MEM_CONTEXT                              =0x1106;
exports.MEM_ASSOCIATED_MEMOBJECT                 =0x1107;
exports.MEM_OFFSET                               =0x1108;

  /* cl_image_info */
exports.IMAGE_FORMAT                             =0x1110;
exports.IMAGE_ELEMENT_SIZE                       =0x1111;
exports.IMAGE_ROW_PITCH                          =0x1112;
exports.IMAGE_WIDTH                              =0x1114;
exports.IMAGE_HEIGHT                             =0x1115;

  /* cl_addressing_mode */
exports.ADDRESS_NONE                             =0x1130;
exports.ADDRESS_CLAMP_TO_EDGE                    =0x1131;
exports.ADDRESS_CLAMP                            =0x1132;
exports.ADDRESS_REPEAT                           =0x1133;
exports.ADDRESS_MIRRORED_REPEAT                  =0x1134;

  /* cl_filter_mode */
exports.FILTER_NEAREST                           =0x1140;
exports.FILTER_LINEAR                            =0x1141;

  /* cl_sampler_info */
exports.SAMPLER_REFERENCE_COUNT                  =0x1150;
exports.SAMPLER_CONTEXT                          =0x1151;
exports.SAMPLER_NORMALIZED_COORDS                =0x1152;
exports.SAMPLER_ADDRESSING_MODE                  =0x1153;
exports.SAMPLER_FILTER_MODE                      =0x1154;

  /* cl_map_flags - bitfield */
exports.MAP_READ                                 =(1 << 0);
exports.MAP_WRITE                                =(1 << 1);

  /* cl_program_info */
exports.PROGRAM_REFERENCE_COUNT                  =0x1160;
exports.PROGRAM_CONTEXT                          =0x1161;
exports.PROGRAM_NUM_DEVICES                      =0x1162;
exports.PROGRAM_DEVICES                          =0x1163;
exports.PROGRAM_SOURCE                           =0x1164;
exports.PROGRAM_BINARY_SIZES                     =0x1165;
exports.PROGRAM_BINARIES                         =0x1166;

  /* cl_program_build_info */
exports.PROGRAM_BUILD_STATUS                     =0x1181;
exports.PROGRAM_BUILD_OPTIONS                    =0x1182;
exports.PROGRAM_BUILD_LOG                        =0x1183;

  /* cl_build_status */
exports.BUILD_SUCCESS                             =0;
exports.BUILD_NONE                                =1;
exports.BUILD_ERROR                              =-2;
exports.BUILD_IN_PROGRESS                        =-3;

  /* cl_kernel_info */
exports.KERNEL_FUNCTION_NAME                     =0x1190;
exports.KERNEL_NUM_ARGS                          =0x1191;
exports.KERNEL_REFERENCE_COUNT                   =0x1192;
exports.KERNEL_CONTEXT                           =0x1193;
exports.KERNEL_PROGRAM                           =0x1194;

  /* cl_kernel_work_group_info */
exports.KERNEL_WORK_GROUP_SIZE                   =0x11B0;
exports.KERNEL_COMPILE_WORK_GROUP_SIZE           =0x11B1;
exports.KERNEL_LOCAL_MEM_SIZE                    =0x11B2;
exports.KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE =0x11B3;
exports.KERNEL_PRIVATE_MEM_SIZE                  =0x11B4;

  /* cl_event_info  */
exports.EVENT_COMMAND_QUEUE                      =0x11D0;
exports.EVENT_COMMAND_TYPE                       =0x11D1;
exports.EVENT_REFERENCE_COUNT                    =0x11D2;
exports.EVENT_COMMAND_EXECUTION_STATUS           =0x11D3;
exports.EVENT_CONTEXT                            =0x11D4;

  /* cl_command_type */
exports.COMMAND_NDRANGE_KERNEL                   =0x11F0;
exports.COMMAND_TASK                             =0x11F1;
exports.COMMAND_NATIVE_KERNEL                    =0x11F2;
exports.COMMAND_READ_BUFFER                      =0x11F3;
exports.COMMAND_WRITE_BUFFER                     =0x11F4;
exports.COMMAND_COPY_BUFFER                      =0x11F5;
exports.COMMAND_READ_IMAGE                       =0x11F6;
exports.COMMAND_WRITE_IMAGE                      =0x11F7;
exports.COMMAND_COPY_IMAGE                       =0x11F8;
exports.COMMAND_COPY_IMAGE_TO_BUFFER             =0x11F9;
exports.COMMAND_COPY_BUFFER_TO_IMAGE             =0x11FA;
exports.COMMAND_MAP_BUFFER                       =0x11FB;
exports.COMMAND_MAP_IMAGE                        =0x11FC;
exports.COMMAND_UNMAP_MEM_OBJECT                 =0x11FD;
exports.COMMAND_MARKER                           =0x11FE;
exports.COMMAND_ACQUIRE_GL_OBJECTS               =0x11FF;
exports.COMMAND_RELEASE_GL_OBJECTS               =0x1200;
exports.COMMAND_READ_BUFFER_RECT                 =0x1201;
exports.COMMAND_WRITE_BUFFER_RECT                =0x1202;
exports.COMMAND_COPY_BUFFER_RECT                 =0x1203;
exports.COMMAND_USER                             =0x1204;

  /* command execution status */
exports.COMPLETE                                 =0x0;
exports.RUNNING                                  =0x1;
exports.SUBMITTED                                =0x2;
exports.QUEUED                                   =0x3;
  
  /* cl_buffer_create_type  */
exports.BUFFER_CREATE_TYPE_REGION                =0x1220;

  /* cl_profiling_info  */
exports.PROFILING_COMMAND_QUEUED                 =0x1280;
exports.PROFILING_COMMAND_SUBMIT                 =0x1281;
exports.PROFILING_COMMAND_START                  =0x1282;
exports.PROFILING_COMMAND_END                    =0x1283;

exports.types = {
};

var cnt = 0;
exports.types.UNKNOWN = cnt++;

exports.types.BYTE = cnt++;
exports.types.CHAR = cnt++;
exports.types.UCHAR = cnt++;
exports.types.SHORT = cnt++;
exports.types.USHORT = cnt++;

    // Basic types
exports.types.INT = cnt++;                        // cl_int
exports.types.UINT = cnt++;                       // cl_uint
exports.types.LONG = cnt++;                       // cl_long
exports.types.ULONG = cnt++;                      // cl_ulong
exports.types.BOOL = cnt++;                       // cl_bool = cl_uint                    //10
exports.types.SIZE_T = cnt++;                     // size_t
exports.types.HALF = cnt++;                       // cl_half
exports.types.FLOAT = cnt++;                      // cl_float
exports.types.DOUBLE = cnt++;                     // cl_double

// String types
exports.types.STRING = cnt++;                     // char*

    // Class types
exports.types.PLATFORM = cnt++;                   // cl_platform_id
exports.types.DEVICE = cnt++;                     // cl_device_id
exports.types.CONTEXT = cnt++;                    // cl_context
exports.types.COMMAND_QUEUE = cnt++;              // cl_command_queue
exports.types.MEMORY_OBJECT = cnt++;              // cl_mem                               //20
exports.types.MEM = exports.types.MEMORY_OBJECT;
exports.types.PROGRAM = cnt++;                    // cl_program
exports.types.KERNEL = cnt++;                     // cl_kernel
exports.types.EVENT = cnt++;                      // cl_event
exports.types.SAMPLER = cnt++;                    // cl_sampler

exports.types.IMAGE_FORMAT = cnt++;

    // Special types
exports.types.ADRESSING_MODE = cnt++;             // cl_addressing_mode
exports.types.BUILD_STATUS = cnt++;               // cl_build_status
exports.types.CHANNEL_ORDER = cnt++;              // cl_channel_order
exports.types.CHANNEL_TYPE = cnt++;               // cl_channel_type
exports.types.COMMAND_QUEUE_PROPERTIES = cnt++;   // cl_command_queue_properties          //30
exports.types.COMMAND_TYPE = cnt++;               // cl_command_type
exports.types.CONTEXT_PROPERTIES = cnt++;         // cl_context_properties

exports.types.DEVICE_EXEC_CAPABILITIES = cnt++;   // cl_device_exec_capabilities
exports.types.DEVICE_FP_CONFIG = cnt++;           // cl_device_fp_config
exports.types.DEVICE_LOCAL_MEM_TYPE = cnt++;      // cl_device_local_mem_type
exports.types.DEVICE_MEM_CACHE_TYPE = cnt++;      // cl_device_mem_cache_type
exports.types.DEVICE_TYPE = cnt++;                // cl_device_type
exports.types.FILTER_MODE = cnt++;                // cl_filter_mode
exports.types.GL_OBJECT_TYPE = cnt++;             // cl_gl_object_type
exports.types.MAP_FLAGS = cnt++;                  // cl_map_flags                         //40
exports.types.MEM_FENCE_FLAGS = cnt++;            // cl_mem_fence_flags
exports.types.MEM_FLAGS = cnt++;                  // cl_mem_flags
exports.types.MEM_OBJECT_TYPE = cnt++;            // cl_mem_object_type

    // Vector types
exports.types.BYTE_V = cnt++;
exports.types.CHAR_V = cnt++;
exports.types.UCHAR_V = cnt++;
exports.types.SHORT_V = cnt++;
exports.types.USHORT_V = cnt++;
exports.types.INT_V = cnt++;                      // cl_int*
exports.types.UINT_V = cnt++;                     // cl_uint*                             //50
exports.types.LONG_V = cnt++;                     // cl_long*
exports.types.ULONG_V = cnt++;                    // cl_ulong*
exports.types.BOOL_V = cnt++;                     // cl_bool*
exports.types.SIZE_T_V = cnt++;                   // size_t*
exports.types.HALF_V = cnt++;
exports.types.FLOAT_V = cnt++;
exports.types.DOUBLE_V = cnt++;
exports.types.STRING_V = cnt++;                   // char**

exports.types.PLATFORM_V = cnt++;
exports.types.DEVICE_V = cnt++;                                                           //60
exports.types.CONTEXT_V = cnt++;
exports.types.COMMAND_QUEUE_V = cnt++;
exports.types.MEMORY_OBJECT_V = cnt++;
exports.types.PROGRAM_V = cnt++;
exports.types.KERNEL_V = cnt++;
exports.types.EVENT_V = cnt++;
exports.types.SAMPLER_V = cnt++;

exports.types.LAST = cnt;
