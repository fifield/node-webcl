#!/usr/bin/env node

var cl = require('webcl');

var platforms = cl.getPlatforms();
for (var i=0; i<platforms.length; i++) {
    console.log("Platform "+i);
    console.log("  "+platforms[i].getInfo(cl.PLATFORM_PROFILE));
    console.log("  "+platforms[i].getInfo(cl.PLATFORM_VERSION));
    console.log("  "+platforms[i].getInfo(cl.PLATFORM_NAME));
    console.log("  "+platforms[i].getInfo(cl.PLATFORM_VENDOR));
    console.log("  "+platforms[i].getInfo(cl.PLATFORM_EXTENSIONS));

    var devices = platforms[i].getDevices(cl.DEVICE_TYPE_ALL);
    for (var j=0; j<devices.length; j++) {
	console.log("  Device "+j);
	console.log("    "+devices[j].getInfo(cl.DEVICE_NAME));
	var device_type = devices[j].getInfo(cl.DEVICE_TYPE);
	if (device_type & cl.DEVICE_TYPE_CPU) console.log("    CPU");
	if (device_type & cl.DEVICE_TYPE_GPU) console.log("    GPU");
    }
}

//
// gpu contexts
//

var devices = platforms[0].getDevices(cl.DEVICE_TYPE_GPU);
var gpu_ctx = cl.createContext([cl.CONTEXT_PLATFORM, platforms[0]], devices);    

console.log("GPU Context devices: "+gpu_ctx.getInfo(cl.CONTEXT_NUM_DEVICES) );
console.log("GPU Context refcnt: "+gpu_ctx.getInfo(cl.CONTEXT_REFERENCE_COUNT) );

devices = gpu_ctx.getInfo(cl.CONTEXT_DEVICES);
for (var j=0; j<devices.length; j++) {
    console.log("  Device "+j);
    console.log("    "+devices[j].getInfo(cl.DEVICE_NAME));
    var device_type = devices[j].getInfo(cl.DEVICE_TYPE);
    if (device_type & cl.DEVICE_TYPE_CPU) console.log("    CPU");
    if (device_type & cl.DEVICE_TYPE_GPU) console.log("    GPU");
}

//
// cpu contexts
//

devices = platforms[0].getDevices(cl.DEVICE_TYPE_GPU);
var cpu_ctx = cl.createContextFromType([cl.CONTEXT_PLATFORM, platforms[0]],
				       cl.DEVICE_TYPE_CPU);    

console.log("CPU Context devices: "+cpu_ctx.getInfo(cl.CONTEXT_NUM_DEVICES) );
console.log("CPU Context refcnt: "+cpu_ctx.getInfo(cl.CONTEXT_REFERENCE_COUNT) );

devices = cpu_ctx.getInfo(cl.CONTEXT_DEVICES);
for (var j=0; j<devices.length; j++) {
    console.log("  Device "+j);
    console.log("    "+devices[j].getInfo(cl.DEVICE_NAME));
    var device_type = devices[j].getInfo(cl.DEVICE_TYPE);
    if (device_type & cl.DEVICE_TYPE_CPU) console.log("    CPU");
    if (device_type & cl.DEVICE_TYPE_GPU) console.log("    GPU");
}

//
// 
// 
