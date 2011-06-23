project(ITK_Expat)
set(ITK_Expat_THIRD_PARTY 1)

set(ITK_Expat_INCLUDE_DIRS
  ${ITK_Expat_BINARY_DIR}/src/expat
  ${ITK_Expat_SOURCE_DIR}/src/expat
  )
set(ITK_Expat_LIBRARIES ITKEXPAT)

itk_module_impl()
