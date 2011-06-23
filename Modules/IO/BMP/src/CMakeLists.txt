set(ITK_IO_BMP_SRC
itkBMPImageIO.cxx
itkBMPImageIOFactory.cxx
)

add_library(ITK_IO_BMP ${ITK_IO_BMP_SRC})
target_link_libraries(ITK_IO_BMP  ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_BMP)
