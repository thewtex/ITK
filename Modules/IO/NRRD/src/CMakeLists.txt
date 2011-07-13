set(ITKIONRRD_SRC
itkNrrdImageIOFactory.cxx
itkNrrdImageIO.cxx
)

add_library(ITKIONRRD ${ITKIONRRD_SRC})
target_link_libraries(ITKIONRRD  ${ITK-NrrdIO_LIBRARIES} ${ITKIOBase_LIBRARIES})
itk_module_target(ITKIONRRD)
