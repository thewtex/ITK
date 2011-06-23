set(ITK_VTK_SRC
itkVTKImageExportBase.cxx
)

add_library(ITK_VTK ${ITK_VTK_SRC})
target_link_libraries(ITK_VTK  ${ITK_Common_LIBRARIES})
itk_module_target(ITK_VTK)
