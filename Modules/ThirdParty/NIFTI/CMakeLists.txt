project(ITK_NIFTI)
set(ITK_NIFTI_THIRD_PARTY 1)

set(ITK_NIFTI_INCLUDE_DIRS
  ${ITK_NIFTI_SOURCE_DIR}/src/nifti/niftilib
  ${ITK_NIFTI_SOURCE_DIR}/src/nifti/znzlib
  )
set(ITK_NIFTI_LIBRARIES ITKznz ITKniftiio)

itk_module_impl()
