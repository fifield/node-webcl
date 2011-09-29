#!/usr/bin/env node

var WebCL = require('webcl');

var alert = console.log;

function dumpCLData () {
  var s = "";
  try {
    // First check if the WebCL extension is installed at all
                
    if (WebCL == undefined) {
      alert("Unfortunately your system does not support WebCL. " +
	    "Make sure that you have the OpenCL extension installed.");
      return;
    }
    
    // List of OpenCL information parameter names.

    var infos = [ [ "CL_DEVICE_ADDRESS_BITS", WebCL.CL_DEVICE_ADDRESS_BITS ],
      [ "CL_DEVICE_AVAILABLE", WebCL.CL_DEVICE_AVAILABLE ],
      [ "CL_DEVICE_COMPILER_AVAILABLE", WebCL.CL_DEVICE_COMPILER_AVAILABLE ],
      [ "CL_DEVICE_DOUBLE_FP_CONFIG", WebCL.CL_DEVICE_DOUBLE_FP_CONFIG ],
      [ "CL_DEVICE_ENDIAN_LITTLE", WebCL.CL_DEVICE_ENDIAN_LITTLE ],
      [ "CL_DEVICE_ERROR_CORRECTION_SUPPORT", WebCL.CL_DEVICE_ERROR_CORRECTION_SUPPORT ],
      [ "CL_DEVICE_EXECUTION_CAPABILITIES", WebCL.CL_DEVICE_EXECUTION_CAPABILITIES ],
      [ "CL_DEVICE_EXTENSIONS", WebCL.CL_DEVICE_EXTENSIONS ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", WebCL.CL_DEVICE_GLOBAL_MEM_CACHE_SIZE ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", WebCL.CL_DEVICE_GLOBAL_MEM_CACHE_TYPE ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", WebCL.CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE ],
      [ "CL_DEVICE_GLOBAL_MEM_SIZE", WebCL.CL_DEVICE_GLOBAL_MEM_SIZE ],
      [ "CL_DEVICE_HALF_FP_CONFIG", WebCL.CL_DEVICE_HALF_FP_CONFIG ],
      [ "CL_DEVICE_IMAGE_SUPPORT", WebCL.CL_DEVICE_IMAGE_SUPPORT ],
      [ "CL_DEVICE_IMAGE2D_MAX_HEIGHT", WebCL.CL_DEVICE_IMAGE2D_MAX_HEIGHT ],
      [ "CL_DEVICE_IMAGE2D_MAX_WIDTH", WebCL.CL_DEVICE_IMAGE2D_MAX_WIDTH ],
      [ "CL_DEVICE_IMAGE3D_MAX_DEPTH", WebCL.CL_DEVICE_IMAGE3D_MAX_DEPTH ],
      [ "CL_DEVICE_IMAGE3D_MAX_HEIGHT", WebCL.CL_DEVICE_IMAGE3D_MAX_HEIGHT ],
      [ "CL_DEVICE_IMAGE3D_MAX_WIDTH", WebCL.CL_DEVICE_IMAGE3D_MAX_WIDTH ],
      [ "CL_DEVICE_LOCAL_MEM_SIZE", WebCL.CL_DEVICE_LOCAL_MEM_SIZE ],
      [ "CL_DEVICE_LOCAL_MEM_TYPE", WebCL.CL_DEVICE_LOCAL_MEM_TYPE ],
      [ "CL_DEVICE_MAX_CLOCK_FREQUENCY", WebCL.CL_DEVICE_MAX_CLOCK_FREQUENCY ],
      [ "CL_DEVICE_MAX_COMPUTE_UNITS", WebCL.CL_DEVICE_MAX_COMPUTE_UNITS ],
      [ "CL_DEVICE_MAX_CONSTANT_ARGS", WebCL.CL_DEVICE_MAX_CONSTANT_ARGS ],
      [ "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", WebCL.CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE ],
      [ "CL_DEVICE_MAX_MEM_ALLOC_SIZE", WebCL.CL_DEVICE_MAX_MEM_ALLOC_SIZE ],
      [ "CL_DEVICE_MAX_PARAMETER_SIZE", WebCL.CL_DEVICE_MAX_PARAMETER_SIZE ],
      [ "CL_DEVICE_MAX_READ_IMAGE_ARGS", WebCL.CL_DEVICE_MAX_READ_IMAGE_ARGS ],
      [ "CL_DEVICE_MAX_SAMPLERS", WebCL.CL_DEVICE_MAX_SAMPLERS ],
      [ "CL_DEVICE_MAX_WORK_GROUP_SIZE", WebCL.CL_DEVICE_MAX_WORK_GROUP_SIZE ],
      [ "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", WebCL.CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS ],
      [ "CL_DEVICE_MAX_WORK_ITEM_SIZES", WebCL.CL_DEVICE_MAX_WORK_ITEM_SIZES ],
      [ "CL_DEVICE_MAX_WRITE_IMAGE_ARGS", WebCL.CL_DEVICE_MAX_WRITE_IMAGE_ARGS ],
      [ "CL_DEVICE_MEM_BASE_ADDR_ALIGN", WebCL.CL_DEVICE_MEM_BASE_ADDR_ALIGN ],
      [ "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE", WebCL.CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE ],
      [ "CL_DEVICE_NAME", WebCL.CL_DEVICE_NAME ],
      [ "CL_DEVICE_PLATFORM", WebCL.CL_DEVICE_PLATFORM ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", WebCL.CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE ],
      [ "CL_DEVICE_PROFILE", WebCL.CL_DEVICE_PROFILE ],
      [ "CL_DEVICE_PROFILING_TIMER_RESOLUTION", WebCL.CL_DEVICE_PROFILING_TIMER_RESOLUTION ],
      [ "CL_DEVICE_QUEUE_PROPERTIES", WebCL.CL_DEVICE_QUEUE_PROPERTIES ],
      [ "CL_DEVICE_SINGLE_FP_CONFIG", WebCL.CL_DEVICE_SINGLE_FP_CONFIG ],
      [ "CL_DEVICE_TYPE", WebCL.CL_DEVICE_TYPE ],
      [ "CL_DEVICE_VENDOR", WebCL.CL_DEVICE_VENDOR ],
      [ "CL_DEVICE_VENDOR_ID", WebCL.CL_DEVICE_VENDOR_ID ],
      [ "CL_DEVICE_VERSION", WebCL.CL_DEVICE_VERSION ],
      [ "CL_DRIVER_VERSION", WebCL.CL_DRIVER_VERSION ] ];
    
    
    // Get a list of available CL platforms, and another list of the
    // available devices on each platform. Platform and device information 
    // is inquired into string s.

    var platforms = WebCL.getPlatformIDs ();
    s += "Found " + platforms.length + " platform"
        + (platforms.length == 1 ? "" : "s")
        + "." + "\n\n";
    for (var i in platforms) {
      var plat = platforms[i];

      var name = plat.getPlatformInfo (WebCL.CL_PLATFORM_NAME);
      s += "[" + i + "] \"" + name + "\"\n";
      s += "vendor: " 
        + plat.getPlatformInfo (WebCL.CL_PLATFORM_VENDOR) + "\n";
      s += "version: " 
        + plat.getPlatformInfo (WebCL.CL_PLATFORM_VERSION) + "\n";
      s += "profile: " 
        + plat.getPlatformInfo (WebCL.CL_PLATFORM_PROFILE) + "\n";
      s += "extensions: " 
        + plat.getPlatformInfo (WebCL.CL_PLATFORM_EXTENSIONS) + "\n";

      var devices = plat.getDeviceIDs (WebCL.CL_DEVICE_TYPE_ALL);
      s += "Devices: " + devices.length + "\n";
      for (var j in devices) {
        var dev = devices[j];
        s += "[" + j + "] \"" + dev.getDeviceInfo(WebCL.CL_DEVICE_NAME) 
          + "\"\n";

        for (var k in infos) {
          s += infos[k][0] + ":   ";
          try {
            if (infos[k][1] == WebCL.CL_DEVICE_PLATFORM) {
              s += "" 
                + dev.getDeviceInfo(infos[k][1]).getPlatformInfo(WebCL.CL_PLATFORM_NAME) 
                + "";
            } else {
              s += "" + dev.getDeviceInfo(infos[k][1]) + "";
            }
          } catch (e) {
            s += "Info not available";
          }
          s += "\n";
        }
      }
    }
    
    console.log(s);
  } catch(e) {
    console.log(s + "\n" + e.toString());
    throw e;
  }
}

 dumpCLData ();