project(ITK_DICOMParser)
set(ITK_DICOMParser_THIRD_PARTY 1)

set(ITK_DICOMParser_INCLUDE_DIRS
  ${ITK_DICOMParser_BINARY_DIR}/src/DICOMParser
  ${ITK_DICOMParser_SOURCE_DIR}/src/DICOMParser
  )
set(ITK_DICOMParser_LIBRARIES ITKDICOMParser)

itk_module_impl()
