project(ITK_OpenJPEG)
set(ITK_OpenJPEG_THIRD_PARTY 1)

set(ITK_OpenJPEG_INCLUDE_DIRS
  ${ITK_OpenJPEG_BINARY_DIR}/src/openjpeg
  ${ITK_OpenJPEG_SOURCE_DIR}/src/openjpeg
  )
set(ITK_OpenJPEG_LIBRARIES itkopenjpeg)

itk_module_impl()
