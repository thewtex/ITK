set(VideoIOOpenCV_SRC
itkOpenCVVideoIO.cxx
itkOpenCVVideoIOFactory.cxx
)

add_library(VideoIOOpenCV ${VideoIOOpenCV_SRC})
target_link_libraries(VideoIOOpenCV ${ITKCommon_LIBRARIES} ${ITKIOBase} VideoIOBase ${TEST_DEPENDS_LIBRARIES} ${ITKTestKernel_LIBRARIES})
itk_module_target(VideoIOOpenCV)
