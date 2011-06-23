set(ITK_IO_NRRD_SRC
itkNrrdImageIOFactory.cxx
itkNrrdImageIO.cxx
)

add_library(ITK_IO_NRRD ${ITK_IO_NRRD_SRC})
target_link_libraries(ITK_IO_NRRD  ${ITK_NrrdIO_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_NRRD)
