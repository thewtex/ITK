#include <assert.h>
#include "itkGPUContextManager.h"

namespace itk
{
  // static variable initialization
  GPUContextManager* GPUContextManager::m_Instance = NULL;

  GPUContextManager* GPUContextManager::GetInstance()
  {
    if(m_Instance == NULL)
    {
      m_Instance = new GPUContextManager();
    }
    return m_Instance;
  }

  void GPUContextManager::DestroyInstance()
  {
    delete m_Instance;
    m_Instance = NULL;
  }

  GPUContextManager::GPUContextManager()
  {
    cl_int errid;

    // Get the platforms
    errid = clGetPlatformIDs(0, NULL, &m_NumPlatforms);
    OclCheckError( errid );

    // TODO: add support for ATI and INTEL
    // Get NVIDIA platform by default
    m_Platform = OclSelectPlatform("NVIDIA");
    assert(m_Platform != NULL);

    cl_device_type devType = CL_DEVICE_TYPE_GPU; // CL_DEVICE_TYPE_CPU

    // TODO: add support for CPU device type
    // Get the devices
    m_Devices = OclGetAvailableDevices(m_Platform, devType, &m_NumDevices);

    /*
    errid = clGetDeviceIDs(m_Platform,devType, 0, NULL, &m_NumDevices);
    OclCheckError( errid );

    std::cout << "# of device : " << m_NumDevices << std::endl;

    m_Devices = (cl_device_id *)malloc(m_NumDevices * sizeof(cl_device_id) );
    errid = clGetDeviceIDs(m_Platform, devType, m_NumDevices, m_Devices, NULL);
    OclCheckError( errid );

    std::cout << "We got devices" << std::endl;
    */

    // create context
    m_Context = clCreateContext(0, m_NumDevices, m_Devices, NULL, NULL, &errid);
    OclCheckError( errid );

    // create command queues
    m_CommandQueue = (cl_command_queue *)malloc(m_NumDevices * sizeof(cl_command_queue));
    for(unsigned int i=0; i<m_NumDevices; i++)
    {
      m_CommandQueue[i] = clCreateCommandQueue(m_Context, m_Devices[i], 0, &errid);

      OclPrintDeviceName(m_Devices[i]);

      OclCheckError( errid );
    }

    //m_current_command_queue_id = 0; // default command queue id
  }


  GPUContextManager::~GPUContextManager()
  {
    // TODO: check if this is correct
    free( m_Platform );
    free( m_Devices );
  }


  cl_command_queue GPUContextManager::GetCommandQueue(int i)
  {
    if(i < 0 || i >= (int)m_NumDevices)
    {
      printf("Error: requested queue id is not available. Default queue will be used (queue id = 0)\n");
      return m_CommandQueue[0];
    }
    return m_CommandQueue[i];
  }


  cl_device_id GPUContextManager::GetDeviceId(int i)
  {
    if(i < 0 || i >= (int)m_NumDevices)
    {
      printf("Error: requested queue id is not available. Default queue will be used (queue id = 0)\n");
      return m_Devices[0];
    }
    return m_Devices[i];
  }

} // namespace itk
