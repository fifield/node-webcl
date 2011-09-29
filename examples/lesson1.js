#!/usr/bin/env node

var WebCL = require('webcl');

var alert = console.log;

function detectCL() {
  // First check if the WebCL extension is installed at all
  
  if (WebCL == undefined) {
      alert("Unfortunately your system does not support WebCL. " +
	    "Make sure that you have the OpenCL extension installed.");
    return;
  }

  // Get a list of available CL platforms, and another list of the
  // available devices on each platform. If there are no platforms,
  // or no available devices on any platform, then we can conclude
  // that WebCL is not available.

  try {
    var platforms = WebCL.getPlatformIDs();
    var devices = [];
    for (var i in platforms) {
      var plat = platforms[i];
      devices[i] = plat.getDeviceIDs(WebCL.CL_DEVICE_TYPE_ALL);
    }
    alert("Excellent! Your system does support WebCL.");
  } catch (e) {
    alert("Unfortunately platform or device inquiry failed.");
  }
}

detectCL();