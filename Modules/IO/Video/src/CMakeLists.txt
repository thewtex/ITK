set(ITKIOVideo_SRC
itkFileListVideoIO.cxx
itkFileListVideoIOFactory.cxx
itkVideoIOBase.cxx
itkVideoIOFactory.cxx
)

add_library(ITKIOVideo ${ITKIOVideo_SRC})
target_link_libraries(ITKIOVideo ${ITKCommon_LIBRARIES} ${ITKIOBase})
itk_module_target(ITKIOVideo)
