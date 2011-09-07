set(VideoIOVXL_SRC
itkVXLVideoIO.cxx
itkVXLVideoIOFactory.cxx
)

add_library(VideoIOVXL ${VideoIOVXL_SRC})
target_link_libraries(VideoIOVXL ${ITKCommon_LIBRARIES} ${ITKIOBase} VideoIOBase ${TEST_DEPENDS_LIBRARIES} ${ITKTestKernel_LIBRARIES})
itk_module_target(VideoIOVXL)
