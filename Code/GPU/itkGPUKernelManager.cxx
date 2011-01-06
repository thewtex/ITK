/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkGPUKernelManager.h"

namespace itk
{
GPUKernelManager::GPUKernelManager()
{
m_manager = GPUContextManager::getInstance();

if(m_manager->getNumCommandQueue() > 0)m_command_queue_id = 0; // default command queue
}

bool GPUKernelManager::LoadProgramFromFile(const char* filename, const char* cPreamble)
{
// locals
FILE* pFileStream = NULL;
size_t szSourceLength;
size_t szFinalLength;

// open the OpenCL source code file
#ifdef _WIN32   // Windows version
if(fopen_s(&pFileStream, filename, "rb") != 0)
{
itkWarningMacro("Cannot open OpenCL source file");
return false;
}
#else           // Linux version
pFileStream = fopen(filename, "rb");
if(pFileStream == 0)
{
return NULL;
}
#endif

size_t szPreambleLength = strlen(cPreamble);

// get the length of the source code
fseek(pFileStream, 0, SEEK_END);
szSourceLength = ftell(pFileStream);
fseek(pFileStream, 0, SEEK_SET);

// allocate a buffer for the source code string and read it in
char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
if(szPreambleLength > 0) memcpy(cSourceString, cPreamble, szPreambleLength);
if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
{
fclose(pFileStream);
free(cSourceString);
return false;
}

// close the file and return the total length of the combined (preamble + source) string
fclose(pFileStream);

szFinalLength = szSourceLength + szPreambleLength;

cSourceString[szSourceLength + szPreambleLength] = '\0';

//
// Create OpenCL program from source strings
//
cl_int errid;
m_program = clCreateProgramWithSource(m_manager->getCurrentContext(), 1, (const char **)&cSourceString, &szFinalLength, &errid);
oclCheckError(errid);
if(errid != CL_SUCCESS)
{
itkWarningMacro("Cannot create GPU program");
return false;
}

// build program
errid = clBuildProgram(m_program, 0, NULL, NULL, NULL, NULL);
oclCheckError(errid);
if(errid != CL_SUCCESS)
{
itkWarningMacro("OpenCL program build error");
return false;
}

return true;
}


int GPUKernelManager::CreateKernel(const char* kernelName)
{
cl_int errid;

// create kernel
cl_kernel newKernel = clCreateKernel(m_program, kernelName, &errid);
oclCheckError(errid);

if(errid != CL_SUCCESS) // failed
{
itkWarningMacro("Fail to create GPU kernel");
return -1;
}

m_kernelContainer.push_back( newKernel );

// argument list
m_kernelArgumentReady.push_back( std::vector< KernelArgumentList >() );
cl_uint nArg;
errid = clGetKernelInfo( newKernel, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &nArg, NULL);
(m_kernelArgumentReady.back()).resize( nArg );

ResetArguments( (int)m_kernelContainer.size()-1 );

return (int)m_kernelContainer.size()-1;
}


bool GPUKernelManager::SetKernelArg(int kernelIdx, cl_uint argIdx, size_t argSize, const void* argVal)
{
if(kernelIdx < 0 || kernelIdx >= (int)m_kernelContainer.size()) return false;

cl_int errid;

errid = clSetKernelArg(m_kernelContainer[kernelIdx], argIdx, argSize, argVal);
oclCheckError(errid);

m_kernelArgumentReady[kernelIdx][argIdx].m_isReady = true;
m_kernelArgumentReady[kernelIdx][argIdx].m_GPUDataManager = (GPUDataManager::Pointer)NULL;

return true;
}


bool GPUKernelManager::SetKernelArgWithImage(int kernelIdx, cl_uint argIdx, GPUDataManager::Pointer manager)
{
if(kernelIdx < 0 || kernelIdx >= (int)m_kernelContainer.size()) return false;

cl_int errid;

errid = clSetKernelArg(m_kernelContainer[kernelIdx], argIdx, sizeof(cl_mem), manager->GetGPUBufferPointer());
oclCheckError(errid);

m_kernelArgumentReady[kernelIdx][argIdx].m_isReady = true;
m_kernelArgumentReady[kernelIdx][argIdx].m_GPUDataManager = manager;

return true;
}

// this function must be called right before GPU kernel is launched
bool GPUKernelManager::CheckArgumentReady(int kernelIdx)
{
int nArg = m_kernelArgumentReady[kernelIdx].size();

for(int i=0; i<nArg; i++)
{
if(!(m_kernelArgumentReady[kernelIdx][i].m_isReady)) return false;

// automatic synchronization before kernel launch
if(m_kernelArgumentReady[kernelIdx][i].m_GPUDataManager != (GPUDataManager::Pointer)NULL)
{
m_kernelArgumentReady[kernelIdx][i].m_GPUDataManager->SetCPUBufferDirty();
}
}
return true;
}

void GPUKernelManager::ResetArguments(int kernelIdx)
{
int nArg = m_kernelArgumentReady[kernelIdx].size();

for(int i=0; i<nArg; i++)
{
m_kernelArgumentReady[kernelIdx][i].m_isReady = false;
m_kernelArgumentReady[kernelIdx][i].m_GPUDataManager = (GPUDataManager::Pointer)NULL;
}
}

bool GPUKernelManager::LaunchKernel1D(int kernelIdx, size_t globalWorkSize, size_t localWorkSize)
{
if(kernelIdx < 0 || kernelIdx >= (int)m_kernelContainer.size()) return false;

if(!CheckArgumentReady(kernelIdx))
{
itkWarningMacro("GPU kernel arguments are not completely assigned");
return false;
}

cl_int errid;
errid = clEnqueueNDRangeKernel(m_manager->getCommandQueue(m_command_queue_id), m_kernelContainer[kernelIdx], 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
oclCheckError(errid);

if(errid != CL_SUCCESS)
{
itkWarningMacro("GPU kernel launch failed");
return false;
}

return true;
}


bool GPUKernelManager::LaunchKernel2D(int kernelIdx,
size_t globalWorkSizeX, size_t globalWorkSizeY,
size_t localWorkSizeX,  size_t localWorkSizeY )
{
if(kernelIdx < 0 || kernelIdx >= (int)m_kernelContainer.size()) return false;

if(!CheckArgumentReady(kernelIdx))
{
itkWarningMacro("GPU kernel arguments are not completely assigned");
return false;
}

size_t gws[2], lws[2];

gws[0] = globalWorkSizeX;
gws[1] = globalWorkSizeY;

lws[0] = localWorkSizeX;
lws[1] = localWorkSizeY;

cl_int errid;
errid = clEnqueueNDRangeKernel(m_manager->getCommandQueue(m_command_queue_id), m_kernelContainer[kernelIdx], 2, NULL, gws, lws, 0, NULL, NULL);
oclCheckError(errid);

if(errid != CL_SUCCESS)
{
itkWarningMacro("GPU kernel launch failed");
return false;
}

return true;
}

bool GPUKernelManager::LaunchKernel3D(int kernelIdx,
size_t globalWorkSizeX, size_t globalWorkSizeY, size_t globalWorkSizeZ,
size_t localWorkSizeX,  size_t localWorkSizeY, size_t localWorkSizeZ )
{
if(kernelIdx < 0 || kernelIdx >= (int)m_kernelContainer.size()) return false;

if(!CheckArgumentReady(kernelIdx))
{
itkWarningMacro("GPU kernel arguments are not completely assigned");
return false;
}

size_t gws[3], lws[3];

gws[0] = globalWorkSizeX;
gws[1] = globalWorkSizeY;
gws[2] = globalWorkSizeZ;

lws[0] = localWorkSizeX;
lws[1] = localWorkSizeY;
lws[2] = localWorkSizeZ;

cl_int errid;
errid = clEnqueueNDRangeKernel(m_manager->getCommandQueue(m_command_queue_id), m_kernelContainer[kernelIdx], 3, NULL, gws, lws, 0, NULL, NULL);
oclCheckError(errid);

if(errid != CL_SUCCESS)
{
itkWarningMacro("GPU kernel launch failed");
return false;
}

return true;
}


void GPUKernelManager::SetCurrentCommandQueue( int queueid )
{
if( queueid >= 0 && queueid < (int)m_manager->getNumCommandQueue() )
{
// Assumption: different command queue is assigned to different device
m_command_queue_id = queueid;
}
else
{
itkWarningMacro("Not a valid command queue id");
}
}


int GPUKernelManager::GetCurrentCommandQueueID()
{
return m_command_queue_id;
}

}
