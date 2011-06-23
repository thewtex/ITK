set(ITK_IO_GIPL_SRC
itkGiplImageIOFactory.cxx
itkGiplImageIO.cxx
)

add_library(ITK_IO_GIPL ${ITK_IO_GIPL_SRC})
target_link_libraries(ITK_IO_GIPL  ${ITK_IO_Base_LIBRARIES} ${ITK_ZLIB_LIBRARIES})
itk_module_target(ITK_IO_GIPL)
