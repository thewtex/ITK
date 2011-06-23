project(ITK_NrrdIO)
set(ITK_NrrdIO_THIRD_PARTY 1)

set(ITK_NrrdIO_INCLUDE_DIRS
  ${ITK_NrrdIO_BINARY_DIR}/src/NrrdIO
  ${ITK_NrrdIO_SOURCE_DIR}/src/NrrdIO
  )
set(ITK_NrrdIO_LIBRARIES ITKNrrdIO)

itk_module_impl()
