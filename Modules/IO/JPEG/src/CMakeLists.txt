set(ITKIOJPEG_SRC
itkJPEGImageIOFactory.cxx
itkJPEGImageIO.cxx
)

add_library(ITKIOJPEG ${ITKIOJPEG_SRC})
target_link_libraries(ITKIOJPEG  ${ITK-JPEG_LIBRARIES} ${ITKIOBase_LIBRARIES})
itk_module_target(ITKIOJPEG)
