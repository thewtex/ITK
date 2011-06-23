set(ITK_IO_PNG_SRC
itkPNGImageIOFactory.cxx
itkPNGImageIO.cxx
)

add_library(ITK_IO_PNG ${ITK_IO_PNG_SRC})
target_link_libraries(ITK_IO_PNG  ${ITK_PNG_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_PNG)
