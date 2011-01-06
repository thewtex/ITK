#ifndef __CLUTIL_H__
#define __CLUTIL_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <CL/opencl.h>

//
// Get the device that has the maximum FLOPS in the current context
//
cl_device_id oclGetMaxFlopsDev(cl_context cxGPUContext);

//
// Print device name
//
void oclPrintDeviceName(cl_device_id device);

//
// Find the OpenCL platform that matches the "name"
//
cl_platform_id oclSelectPlatform(char* name);

//
// Check OpenCL error
//
void oclCheckError(cl_int error);

#endif
