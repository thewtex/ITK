option(ITK_USE_GPU "GPU acceleration" OFF)
mark_as_advanced(ITK_USE_GPU)

if(ITK_USE_GPU)
find_package ( OpenCL REQUIRED )
message("GPU acceleration using OpenCL is ON.")

set( itkCxxSRC itkGPUDataManager.cxx itkGPUContextManager.cxx itkGPUKernelManager.cxx itkOclUtil.cxx )

FILE(GLOB itkGPUSRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cl)
FILE(GLOB itkGPUHDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.h)
FILE(GLOB itkTxxSRC  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.txx)

SOURCE_GROUP("OpenCL Source Files" FILES ${itkGPUSRC})

# Include path
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

# Create library
ADD_LIBRARY( ITKGPU ${itkCxxSRC} ${itkTxxSRC} ${itkGPUHDR} )
TARGET_LINK_LIBRARIES( ITKGPU ITKCommon ${OPENCL_LIBRARIES} )

# Test Executable
ADD_EXECUTABLE( test_itkGPUImage test_itkGPUImage.cxx )
TARGET_LINK_LIBRARIES( test_itkGPUImage ITKCommon ITKGPU ${OPENCL_LIBRARIES} )
add_test ( GPUImage test_itkGPUImage )

ADD_EXECUTABLE( test_itkGPUImageFilter test_itkGPUImageFilter.cxx )
TARGET_LINK_LIBRARIES( test_itkGPUImageFilter ITKIO ITKCommon ITKGPU ${OPENCL_LIBRARIES} )
add_test ( GPUImageFilter test_itkGPUImageFilter )

endif(ITK_USE_GPU)
