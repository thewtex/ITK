set(ITK_IO_VTK_SRC
itkVTKImageIOFactory.cxx
itkVTKImageIO.cxx
)

add_library(ITK_IO_VTK ${ITK_IO_VTK_SRC})
target_link_libraries(ITK_IO_VTK  ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_VTK)
