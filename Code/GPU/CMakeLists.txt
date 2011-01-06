message("GPU acceleration using OpenCL is ON.")

set( itkCxxSRC itkGPUDataManager.cxx itkGPUContextManager.cxx itkGPUKernelManager.cxx itkOclUtil.cxx )

FILE(GLOB itkGPUSRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cl)
FILE(GLOB itkGPUHDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.h)
#FILE(GLOB_RECURSE itkCxxSRC  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.cxx)
FILE(GLOB itkTxxSRC  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ *.txx)

SOURCE_GROUP("OpenCL Source Files" FILES ${itkGPUSRC})

FILE(COPY ${itkGPUSRC} DESTINATION ${ITK_BINARY_DIR}/Code/GPU)
MESSAGE( "OpenCL files to copy " ${itkGPUSRC} )

configure_file(
  ${ITK_SOURCE_DIR}/Code/GPU/pathToOpenCLSourceCode.h.in
  ${ITK_BINARY_DIR}/Code/GPU/pathToOpenCLSourceCode.h)

INCLUDE_DIRECTORIES( ${ITK_BINARY_DIR}/Code/GPU )

# ToDo: will be moved to the top CMakeList.txt
SET( OPENCL_INCLUDE_DIR "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/OpenCL/common/inc" )
SET( OPENCL_LINK_DIR "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64" )
SET( OPENCL_LIBRARIES OpenCL.lib )

# Include path
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )
LINK_DIRECTORIES( ${OPENCL_LINK_DIR} )

# Create library
ADD_LIBRARY( ITKGPU ${itkCxxSRC} ${itkTxxSRC} ${itkGPUHDR} )
TARGET_LINK_LIBRARIES( ITKGPU ITKCommon ${OPENCL_LIBRARIES} )

# Test Executable
ADD_EXECUTABLE( test_itkGPUImage test_itkGPUImage.cxx )
TARGET_LINK_LIBRARIES( test_itkGPUImage ITKCommon ITKGPU ${OPENCL_LIBRARIES} )

ADD_EXECUTABLE( test_itkGPUImageFilter test_itkGPUImageFilter.cxx )
TARGET_LINK_LIBRARIES( test_itkGPUImageFilter ITKIO ITKCommon ITKGPU ${OPENCL_LIBRARIES} )
