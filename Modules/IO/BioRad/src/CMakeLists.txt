set(ITK_IO_BioRad_SRC
itkBioRadImageIOFactory.cxx
itkBioRadImageIO.cxx
)

add_library(ITK_IO_BioRad ${ITK_IO_BioRad_SRC})
target_link_libraries(ITK_IO_BioRad  ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_BioRad)
