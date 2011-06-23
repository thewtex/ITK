project(ITK_MetaIO)

set(ITK_MetaIO_INCLUDE_DIRS
  ${ITK_MetaIO_BINARY_DIR}/src/MetaIO
  ${ITK_MetaIO_SOURCE_DIR}/src/MetaIO
  )
set(ITK_MetaIO_LIBRARIES ITKMetaIO)

itk_module_impl()
