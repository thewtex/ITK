set(ITK_IO_LSM_SRC
itkLSMImageIOFactory.cxx
itkLSMImageIO.cxx
)

add_library(ITK_IO_LSM ${ITK_IO_LSM_SRC})
target_link_libraries(ITK_IO_LSM  ${ITK_IO_TIFF_LIBRARIES})
itk_module_target(ITK_IO_LSM)
