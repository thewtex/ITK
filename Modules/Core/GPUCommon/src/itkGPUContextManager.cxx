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
#include <assert.h>
#include "itkGPUContextManager.h"

namespace itk
{
// static variable initialization
GPUContextManager* GPUContextManager::m_Instance = NULL;

GPUContextManager*
GPUContextManager::GetInstance()
{
  if(m_Instance == NULL)
    {
    m_Instance = new GPUContextManager();
    }
  return m_Instance;
}

void
GPUContextManager::DestroyInstance()
{
  std::cout << "Context is destroyed" << std::endl;

  delete m_Instance;
  m_Instance = NULL;
}

GPUContextManager::GPUContextManager()
{
  cl_int errid;

  // Get the platforms
  errid = clGetPlatformIDs(0, NULL, &m_NumberOfPlatforms);
  OpenCLCheckError( errid, __FILE__, __LINE__, ITK_LOCATION );

  // Get NVIDIA platform by default
  m_Platform = OpenCLSelectPlatform("NVIDIA");
  assert(m_Platform != NULL);

  cl_device_type devType = CL_DEVICE_TYPE_GPU; //CL_DEVICE_TYPE_CPU;//

  // Get the devices
  m_Devices = OpenCLGetAvailableDevices(m_Platform, devType, &m_NumberOfDevices);

  // create context
  m_Context = clCreateContext(0, m_NumberOfDevices, m_Devices, NULL, NULL, &errid);
//   m_Context = clCreateContext(0, m_NumberOfDevices, m_Devices, clLogMessagesToStdoutAPPLE, NULL, &errid);

  OpenCLCheckError( errid, __FILE__, __LINE__, ITK_LOCATION );

  // create command queues
  m_CommandQueue = (cl_command_queue *)malloc(m_NumberOfDevices * sizeof(cl_command_queue) );
  for(unsigned int i=0; i<m_NumberOfDevices; i++)
    {
    m_CommandQueue[i] = clCreateCommandQueue(m_Context, m_Devices[i], 0, &errid);

// Debug
    OpenCLPrintDeviceInfo(m_Devices[i], true);
    //
    OpenCLCheckError( errid, __FILE__, __LINE__, ITK_LOCATION );
    }

  //m_current_command_queue_id = 0; // default command queue id
}

GPUContextManager::~GPUContextManager()
{}

cl_command_queue
GPUContextManager::GetCommandQueue(int i)
{
  if(i < 0 || i >= (int)m_NumberOfDevices)
    {
    printf("Error: requested queue id is not available. Default queue will be used (queue id = 0)\n");
    return m_CommandQueue[0];
    }

//std::cout << "Command queue " << i << " is requested " << std::endl;

  return m_CommandQueue[i];
}

cl_device_id
GPUContextManager::GetDeviceId(int i)
{
  if(i < 0 || i >= (int)m_NumberOfDevices)
    {
    printf("Error: requested queue id is not available. Default queue will be used (queue id = 0)\n");
    return m_Devices[0];
    }
  return m_Devices[i];
}

} // namespace itk
