set(ITK-GPUCommon_SRC
itkGPUContextManager.cxx
itkGPUDataManager.cxx
itkGPUKernelManager.cxx
itkOclUtil.cxx
)

  # Create library
  ADD_LIBRARY( ITKGPU ${itkCxxSRC} ${itkTxxSRC} ${itkGPUHDR} )
  TARGET_LINK_LIBRARIES( ITKGPU ITKCommon )

add_library(ITK-GPUCommon ${ITK-GPUCommon_SRC})
target_link_libraries(ITK-GPUCommon ${ITK-Common_LIBRARIES} ${OPENCL_LIBRARIES} )
itk_module_target(ITK-GPUCommon)
