set(DOCUMENTATION "This module contains classes for reading and writing image
files in the MRC file format.")

itk_module(ITKIOMRC
  ENABLE_SHARED
  DEPENDS
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
    ITKImageSources
  DESCRIPTION
    "${DOCUMENTATION}"
)
