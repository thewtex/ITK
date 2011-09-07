set(VideoIOFileList_SRC
itkFileListVideoIO.cxx
itkFileListVideoIOFactory.cxx
)

add_library(VideoIOFileList ${VideoIOFileList_SRC})
target_link_libraries(VideoIOFileList ${ITKCommon_LIBRARIES} ${ITKIOBase} VideoIOBase ${TEST_DEPENDS_LIBRARIES} ${ITKTestKernel_LIBRARIES})
itk_module_target(VideoIOFileList)
