#include <assert.h>
#include "itkGPUContextManager.h"

namespace itk
{
// static variable initialization
GPUContextManager* GPUContextManager::m_pInstance = NULL;

GPUContextManager* GPUContextManager::getInstance()
{
 if(m_pInstance == NULL)
 {
 m_pInstance = new GPUContextManager();
 }
 return m_pInstance;
}

void GPUContextManager::destroyInstance()
{
 delete m_pInstance;
 m_pInstance = NULL;
}

GPUContextManager::GPUContextManager()
{
cl_int errid;

// Get the platforms
errid = clGetPlatformIDs(0, NULL, &m_num_platforms);
oclCheckError( errid );

// TODO: add support for ATI and INTEL
// Get NVIDIA platform by default
m_platform = oclSelectPlatform("NVIDIA");
assert(m_platform != NULL);

// TODO: add support for CPU device type
// Get the devices
errid = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 0, NULL, &m_num_devices);
oclCheckError( errid );

m_devices = (cl_device_id *)malloc(m_num_devices * sizeof(cl_device_id) );
errid = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, m_num_devices, m_devices, NULL);
oclCheckError( errid );

// create context
m_context = clCreateContext(0, m_num_devices, m_devices, NULL, NULL, &errid);
oclCheckError( errid );

// create command queues
m_command_queue = (cl_command_queue *)malloc(m_num_devices * sizeof(cl_command_queue));
for(unsigned int i=0; i<m_num_devices; i++)
{
m_command_queue[i] = clCreateCommandQueue(m_context, m_devices[i], 0, &errid);
//oclPrintDeviceName(m_devices[i]);
oclCheckError( errid );
}

//m_current_command_queue_id = 0; // default command queue id
}


GPUContextManager::~GPUContextManager()
{
// TODO: check if this is correct
free( m_platform );
free( m_devices );
}


cl_command_queue GPUContextManager::getCommandQueue(int i)
{
if(i < 0 || i >= (int)m_num_devices)
{
printf("Error: requested queue id is not available. Default queue will be used (queue id = 0)\n");
return m_command_queue[0];
}
return m_command_queue[i];
}


} // namespace itk
