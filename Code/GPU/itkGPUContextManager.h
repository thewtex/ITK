#ifndef __ITKGPUCONTEXTMANAGER_H__
#define __ITKGPUCONTEXTMANAGER_H__

#include "itkOclUtil.h"
#include <itkLightObject.h>
#include <CL/opencl.h>

//
// Singleton class for GPUContextManager
//

namespace itk
{
class ITK_EXPORT GPUContextManager: public LightObject
{
public:

static GPUContextManager* getInstance();
void destroyInstance();

cl_command_queue getCommandQueue(int i);

unsigned int getNumCommandQueue() { return m_num_devices; }

cl_context getCurrentContext() { return m_context; }

/*
// This has to go to individual GPU object
cl_command_queue getCurrentCommandQueue() { getCommandQueue(m_current_command_queue_id); }
void SetCurrentCommandQueue(unsigned int i) { m_current_command_queue_id = (i >= m_num_devices) ? 0 : i; }
*/

private:

GPUContextManager();
~GPUContextManager();

cl_platform_id     m_platform;
cl_context         m_context;
cl_device_id*      m_devices;
cl_command_queue*  m_command_queue; // one queue per device

cl_uint m_num_devices, m_num_platforms;

static GPUContextManager* m_pInstance;
};
}


#endif
