#!/usr/bin/env node

var WebCL = require('webcl');

var alert = console.log;

var clProgramVectorAdd =
    '__kernel void ckVectorAdd(__global unsigned int* vectorIn1,\n' +
                              '__global unsigned int* vectorIn2,\n' +
                              '__global unsigned int* vectorOut,\n' +
                              'unsigned int uiVectorWidth) {' +
        'unsigned int x = get_global_id(0);' +
        'if (x >= (uiVectorWidth)) return;' +
        'vectorOut[x] = vectorIn1[x] + vectorIn2[x];' +
        'return; }';

function CL_vectorAdd () {

    // All output is written to element by id "output"
    var output = "";

    try {
	
	// First check if the WebCL extension is installed at all 
	if (WebCL == undefined) {
	    alert("Unfortunately your system does not support WebCL. " +
		  "Make sure that you have the WebCL extension installed.");
	    return;
	}

	// Generate input vectors
	var vectorLength = 30;
	var UIvector1 = new Uint32Array(vectorLength);    
	var UIvector2 = new Uint32Array(vectorLength);
	for ( var i=0; i<vectorLength;  i=i+1) {
	    UIvector1[i] = Math.floor(Math.random() * 100); //Random number 0..99
	    UIvector2[i] = Math.floor(Math.random() * 100); //Random number 0..99
	}
	
	output += "\nVector length = " + vectorLength;

	// Setup WebCL context using the default device of the first platform 
	var platforms = WebCL.getPlatforms();
	var ctx = WebCL.createContextFromType ([WebCL.CONTEXT_PLATFORM, 
						platforms[0]],
                                               WebCL.DEVICE_TYPE_GPU);
        
	// Reserve buffers
	var bufSize = vectorLength * 4; // size in bytes
	output += "\nBuffer size: " + bufSize + " bytes";
	var bufIn1 = ctx.createBuffer (WebCL.MEM_READ_ONLY, bufSize);
	var bufIn2 = ctx.createBuffer (WebCL.MEM_READ_ONLY, bufSize);
	var bufOut = ctx.createBuffer (WebCL.MEM_WRITE_ONLY, bufSize);
	
	// Create and build program for the first device
	var kernelSrc = clProgramVectorAdd;
	var program = ctx.createProgram(kernelSrc);
	var devices = ctx.getInfo(WebCL.CONTEXT_DEVICES);

	try {
	    program.build ([devices[0]], "");
	} catch(e) {
	    alert ("Failed to build WebCL program. Error "
		   + program.getBuildInfo (devices[0], 
					   WebCL.PROGRAM_BUILD_STATUS)
		   + ":  " 
		   + program.getBuildInfo (devices[0], 
					   WebCL.PROGRAM_BUILD_LOG));
	    throw e;
	}

	// Create kernel and set arguments
	var kernel = program.createKernel ("ckVectorAdd");
	kernel.setArg (0, bufIn1, WebCL.types.MEM);
	kernel.setArg (1, bufIn2, WebCL.types.MEM);    
	kernel.setArg (2, bufOut, WebCL.types.MEM);
	kernel.setArg (3, vectorLength, WebCL.types.UINT);

	// Create command queue using the first available device
	var cmdQueue = ctx.createCommandQueue (devices[0], 0);

	cmdQueue.enqueueWriteBuffer (bufIn1, false, 0, UIvector1.length*4, UIvector1, []);
	cmdQueue.enqueueWriteBuffer (bufIn2, false, 0, UIvector2.length*4, UIvector2, []);
	
	// Init ND-range
	var localWS = [8];
	var globalWS = [Math.ceil (vectorLength / localWS) * localWS];

	output += "\nGlobal work item size: " + globalWS;
	output += "\nLocal work item size: " + localWS;

	// Execute (enqueue) kernel
	cmdQueue.enqueueNDRangeKernel(kernel, globalWS.length, [], 
                                      globalWS, localWS, []);

	// Read the result buffer from OpenCL device
	var outBuffer = new Uint32Array(vectorLength);
	cmdQueue.enqueueReadBuffer (bufOut, false, 0, outBuffer.length*4, outBuffer, []);
	cmdQueue.finish (); //Finish all the operations

	//Print input vectors and result vector
	output += "\nVector1 = "; 
	for (var i = 0; i < vectorLength; i = i + 1) {
	    output += UIvector1[i] + ", ";
	}
	output += "\nVector2 = ";
	for (var i = 0; i < vectorLength; i = i + 1) {
	    output += UIvector2[i] + ", ";
	}
	output += "\nResult = ";
	for (var i = 0; i < vectorLength; i = i + 1) {
	    output += outBuffer[i] + ", ";
	}

    } catch(e) {
	alert(e.message);
	throw e;
    }
    alert(output);
}

CL_vectorAdd ();