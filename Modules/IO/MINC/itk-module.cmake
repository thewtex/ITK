set(DOCUMENTATION "This module contains classes for reading and writing image
files in the MINC 2 file Format (mnc).")

itk_module(ITKIOMINC
  DEPENDS
    ITKMINC
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
