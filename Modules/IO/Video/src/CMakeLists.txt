set(ITKIOVideo_SRC
itkFileListVideoIO.cxx
itkFileListVideoIOFactory.cxx
itkVideoIOBase.cxx
itkVideoIOFactory.cxx
)

add_library(ITKIOVideo ${ITKIOVideo_SRC})
message(STATUS "${ITKIOVideo_LIBRARIES}")
target_link_libraries(ITKIOVideo ${ITKIOVideo_LIBRARIES})
itk_module_target(ITKIOVideo)
