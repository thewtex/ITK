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

#include "itkGPUDataManager.h"


namespace itk
{
// constructor
GPUDataManager::GPUDataManager()
{
m_manager = GPUContextManager::getInstance();

if(m_manager->getNumCommandQueue() > 0)m_command_queue_id = 0; // default command queue

m_bufferSize = 0;
m_gpu_buffer = NULL;
m_cpu_buffer = NULL;
m_mem_flags  = CL_MEM_READ_WRITE; // default flag
m_isGPUBufferDirty = false;
m_isCPUBufferDirty = false;
}

GPUDataManager::~GPUDataManager()
{
if(m_gpu_buffer) clReleaseMemObject(m_gpu_buffer);
}

void GPUDataManager::SetBufferSize( unsigned int num )
{
m_bufferSize = num;
}

void GPUDataManager::SetBufferFlag( cl_mem_flags flags )
{
m_mem_flags = flags;
}

void GPUDataManager::Allocate()
{
cl_int errid;

if(m_bufferSize > 0)
{
m_gpu_buffer = clCreateBuffer(m_manager->getCurrentContext(), m_mem_flags, m_bufferSize, NULL, &errid);
oclCheckError(errid);
}

m_isGPUBufferDirty = true;

MakeGPUBufferUpToDate();
}

void GPUDataManager::SetCPUBufferPointer( void* ptr )
{
m_cpu_buffer = ptr;
}

void GPUDataManager::SetCPUDirtyFlag( bool isDirty )
{
m_isCPUBufferDirty = isDirty;
}

void GPUDataManager::SetGPUDirtyFlag( bool isDirty )
{
m_isGPUBufferDirty = isDirty;
}

void GPUDataManager::SetGPUBufferDirty()
{
MakeCPUBufferUpToDate();
m_isGPUBufferDirty = true;
}

void GPUDataManager::SetCPUBufferDirty()
{
MakeGPUBufferUpToDate();
m_isCPUBufferDirty = true;
}

void GPUDataManager::MakeCPUBufferUpToDate()
{
if(m_isCPUBufferDirty && m_gpu_buffer != NULL)
{
cl_int errid;

errid = clEnqueueReadBuffer(m_manager->getCommandQueue(m_command_queue_id), m_gpu_buffer, CL_TRUE, 0, m_bufferSize, m_cpu_buffer, 0, NULL, NULL);
oclCheckError(errid);

m_isCPUBufferDirty = false;
}
}


void GPUDataManager::MakeGPUBufferUpToDate()
{
if(m_isGPUBufferDirty && m_cpu_buffer != NULL)
{
cl_int errid;

errid = clEnqueueWriteBuffer(m_manager->getCommandQueue(m_command_queue_id), m_gpu_buffer, CL_TRUE, 0, m_bufferSize, m_cpu_buffer, 0, NULL, NULL);
oclCheckError(errid);

m_isGPUBufferDirty = false;
}
}


cl_mem* GPUDataManager::GetGPUBufferPointer()
{
SetCPUBufferDirty();
return &m_gpu_buffer;
}


bool GPUDataManager::MakeUpToDate()
{
if(m_isGPUBufferDirty && m_isCPUBufferDirty)
{
itkExceptionMacro("Cannot make up-to-date buffer because both CPU and GPU buffers are dirty")
return false;
}

MakeGPUBufferUpToDate();
MakeCPUBufferUpToDate();

m_isGPUBufferDirty = m_isCPUBufferDirty = false;

return true;
}

//
// NOTE: each device has a command queue. Therefore, changing command queue
//       means change a compute device.
//
void GPUDataManager::SetCurrentCommandQueue( int queueid )
{
if( queueid >= 0 && queueid < (int)m_manager->getNumCommandQueue() )
{
MakeCPUBufferUpToDate();

// Assumption: different command queue is assigned to different device
m_command_queue_id = queueid;

m_isGPUBufferDirty = true;
}
else
{
itkWarningMacro("Not a valid command queue id");
}
}

int GPUDataManager::GetCurrentCommandQueueID()
{
return m_command_queue_id;
}

} // namespace itk
