set(ITK_IO_JPEG_SRC
itkJPEGImageIOFactory.cxx
itkJPEGImageIO.cxx
)

add_library(ITK_IO_JPEG ${ITK_IO_JPEG_SRC})
target_link_libraries(ITK_IO_JPEG  ${ITK_JPEG_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_JPEG)
