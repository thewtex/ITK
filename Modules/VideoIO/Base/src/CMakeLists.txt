set(VideoIOBase_SRC
itkVideoIOBase.cxx
itkVideoIOFactory.cxx
)

add_library(VideoIOBase ${VideoIOBase_SRC})
target_link_libraries(VideoIOBase ${ITKCommon_LIBRARIES} ${ITKIOBase} ${TEST_DEPENDS_LIBRARIES} ${ITKTestKernel_LIBRARIES})
itk_module_target(VideoIOBase)
