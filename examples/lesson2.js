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

    var infos = [ [ "CL_DEVICE_ADDRESS_BITS", WebCL.DEVICE_ADDRESS_BITS ],
      [ "CL_DEVICE_AVAILABLE", WebCL.DEVICE_AVAILABLE ],
      [ "CL_DEVICE_COMPILER_AVAILABLE", WebCL.DEVICE_COMPILER_AVAILABLE ],
      [ "CL_DEVICE_DOUBLE_FP_CONFIG", WebCL.DEVICE_DOUBLE_FP_CONFIG ],
      [ "CL_DEVICE_ENDIAN_LITTLE", WebCL.DEVICE_ENDIAN_LITTLE ],
      [ "CL_DEVICE_ERROR_CORRECTION_SUPPORT", WebCL.DEVICE_ERROR_CORRECTION_SUPPORT ],
      [ "CL_DEVICE_EXECUTION_CAPABILITIES", WebCL.DEVICE_EXECUTION_CAPABILITIES ],
      [ "CL_DEVICE_EXTENSIONS", WebCL.DEVICE_EXTENSIONS ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", WebCL.DEVICE_GLOBAL_MEM_CACHE_SIZE ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", WebCL.DEVICE_GLOBAL_MEM_CACHE_TYPE ],
      [ "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", WebCL.DEVICE_GLOBAL_MEM_CACHELINE_SIZE ],
      [ "CL_DEVICE_GLOBAL_MEM_SIZE", WebCL.DEVICE_GLOBAL_MEM_SIZE ],
      [ "CL_DEVICE_HALF_FP_CONFIG", WebCL.DEVICE_HALF_FP_CONFIG ],
      [ "CL_DEVICE_IMAGE_SUPPORT", WebCL.DEVICE_IMAGE_SUPPORT ],
      [ "CL_DEVICE_IMAGE2D_MAX_HEIGHT", WebCL.DEVICE_IMAGE2D_MAX_HEIGHT ],
      [ "CL_DEVICE_IMAGE2D_MAX_WIDTH", WebCL.DEVICE_IMAGE2D_MAX_WIDTH ],
      [ "CL_DEVICE_IMAGE3D_MAX_DEPTH", WebCL.DEVICE_IMAGE3D_MAX_DEPTH ],
      [ "CL_DEVICE_IMAGE3D_MAX_HEIGHT", WebCL.DEVICE_IMAGE3D_MAX_HEIGHT ],
      [ "CL_DEVICE_IMAGE3D_MAX_WIDTH", WebCL.DEVICE_IMAGE3D_MAX_WIDTH ],
      [ "CL_DEVICE_LOCAL_MEM_SIZE", WebCL.DEVICE_LOCAL_MEM_SIZE ],
      [ "CL_DEVICE_LOCAL_MEM_TYPE", WebCL.DEVICE_LOCAL_MEM_TYPE ],
      [ "CL_DEVICE_MAX_CLOCK_FREQUENCY", WebCL.DEVICE_MAX_CLOCK_FREQUENCY ],
      [ "CL_DEVICE_MAX_COMPUTE_UNITS", WebCL.DEVICE_MAX_COMPUTE_UNITS ],
      [ "CL_DEVICE_MAX_CONSTANT_ARGS", WebCL.DEVICE_MAX_CONSTANT_ARGS ],
      [ "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", WebCL.DEVICE_MAX_CONSTANT_BUFFER_SIZE ],
      [ "CL_DEVICE_MAX_MEM_ALLOC_SIZE", WebCL.DEVICE_MAX_MEM_ALLOC_SIZE ],
      [ "CL_DEVICE_MAX_PARAMETER_SIZE", WebCL.DEVICE_MAX_PARAMETER_SIZE ],
      [ "CL_DEVICE_MAX_READ_IMAGE_ARGS", WebCL.DEVICE_MAX_READ_IMAGE_ARGS ],
      [ "CL_DEVICE_MAX_SAMPLERS", WebCL.DEVICE_MAX_SAMPLERS ],
      [ "CL_DEVICE_MAX_WORK_GROUP_SIZE", WebCL.DEVICE_MAX_WORK_GROUP_SIZE ],
      [ "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", WebCL.DEVICE_MAX_WORK_ITEM_DIMENSIONS ],
      [ "CL_DEVICE_MAX_WORK_ITEM_SIZES", WebCL.DEVICE_MAX_WORK_ITEM_SIZES ],
      [ "CL_DEVICE_MAX_WRITE_IMAGE_ARGS", WebCL.DEVICE_MAX_WRITE_IMAGE_ARGS ],
      [ "CL_DEVICE_MEM_BASE_ADDR_ALIGN", WebCL.DEVICE_MEM_BASE_ADDR_ALIGN ],
      [ "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE", WebCL.DEVICE_MIN_DATA_TYPE_ALIGN_SIZE ],
      [ "CL_DEVICE_NAME", WebCL.DEVICE_NAME ],
      [ "CL_DEVICE_PLATFORM", WebCL.DEVICE_PLATFORM ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_CHAR ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_SHORT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_INT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_LONG ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT ],
      [ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", WebCL.DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE ],
      [ "CL_DEVICE_PROFILE", WebCL.DEVICE_PROFILE ],
      [ "CL_DEVICE_PROFILING_TIMER_RESOLUTION", WebCL.DEVICE_PROFILING_TIMER_RESOLUTION ],
      [ "CL_DEVICE_QUEUE_PROPERTIES", WebCL.DEVICE_QUEUE_PROPERTIES ],
      [ "CL_DEVICE_SINGLE_FP_CONFIG", WebCL.DEVICE_SINGLE_FP_CONFIG ],
      [ "CL_DEVICE_TYPE", WebCL.DEVICE_TYPE ],
      [ "CL_DEVICE_VENDOR", WebCL.DEVICE_VENDOR ],
      [ "CL_DEVICE_VENDOR_ID", WebCL.DEVICE_VENDOR_ID ],
      [ "CL_DEVICE_VERSION", WebCL.DEVICE_VERSION ],
      [ "CL_DRIVER_VERSION", WebCL.DRIVER_VERSION ] ];
    
    
    // Get a list of available CL platforms, and another list of the
    // available devices on each platform. Platform and device information 
    // is inquired into string s.

    var platforms = WebCL.getPlatforms ();
    s += "Found " + platforms.length + " platform"
        + (platforms.length == 1 ? "" : "s")
        + "." + "\n\n";
    for (var i in platforms) {
      var plat = platforms[i];

      var name = plat.getInfo (WebCL.PLATFORM_NAME);
      s += "[" + i + "] \"" + name + "\"\n";
      s += "vendor: " 
        + plat.getInfo (WebCL.PLATFORM_VENDOR) + "\n";
      s += "version: " 
        + plat.getInfo (WebCL.PLATFORM_VERSION) + "\n";
      s += "profile: " 
        + plat.getInfo (WebCL.PLATFORM_PROFILE) + "\n";
      s += "extensions: " 
        + plat.getInfo (WebCL.PLATFORM_EXTENSIONS) + "\n";

      var devices = plat.getDevices (WebCL.DEVICE_TYPE_ALL);
      s += "Devices: " + devices.length + "\n";
      for (var j in devices) {
        var dev = devices[j];
        s += "[" + j + "] \"" + dev.getInfo(WebCL.DEVICE_NAME) 
          + "\"\n";

        for (var k in infos) {
          s += infos[k][0] + ":   ";
          try {
            if (infos[k][1] == WebCL.DEVICE_PLATFORM) {
              s += "" 
                + dev.getInfo(infos[k][1]).getInfo(WebCL.PLATFORM_NAME) 
                + "";
            } else {
              s += "" + dev.getInfo(infos[k][1]) + "";
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