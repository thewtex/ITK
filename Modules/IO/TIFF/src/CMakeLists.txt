set(ITK_IO_TIFF_SRC
itkTIFFImageIO.cxx
itkTIFFImageIOFactory.cxx
)

add_library(ITK_IO_TIFF ${ITK_IO_TIFF_SRC})
target_link_libraries(ITK_IO_TIFF  ${ITK_TIFF_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_TIFF)
