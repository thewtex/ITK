set(DOCUMENTATION "This module Implements IO for minc file formats ...")

itk_module(ITKIOMINC
  DEPENDS
    ITKCommon
    ITKIOImageBase
    ITKIOTransformBase
    ITKTransform
    ITKDisplacementField
  TEST_DEPENDS
    ITKTestKernel
    ITKDisplacementField
    ITKIOTransformBase
  DESCRIPTION
    "${DOCUMENTATION}"
)
