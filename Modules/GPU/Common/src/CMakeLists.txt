set(ITK-GPUCommon_SRC
itkGPUContextManager.cxx
itkGPUDataManager.cxx
itkGPUKernelManager.cxx
itkOclUtil.cxx
)

include_directories(${OPENCL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../include/ ${CMAKE_CURRENT_BINARY_DIR})
add_library(ITK-GPUCommon ${ITK-GPUCommon_SRC} ${itkGPUHDR})
target_link_libraries(ITK-GPUCommon ${ITK-Common_LIBRARIES} ${ITK-FiniteDifference_LIBRARIES} ${OPENCL_LIBRARIES} )
itk_module_target(ITK-GPUCommon)
