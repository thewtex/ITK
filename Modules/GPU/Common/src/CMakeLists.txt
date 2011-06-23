set(ITK_GPUCommon_SRC
itkGPUContextManager.cxx
itkGPUDataManager.cxx
itkGPUKernelManager.cxx
itkOclUtil.cxx
)

add_library(ITK_GPUCommon ${ITK_GPUCommon_SRC})
target_link_libraries(ITK_GPUCommon ${ITK_Common_LIBRARIES} ${OPENCL_LIBRARIES} )
itk_module_target(ITK_GPUCommon)
