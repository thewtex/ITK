set(DOCUMENTATION "This module contains the framework for processing images
with the GPU.")

itk_module(ITKGPUCommon
  DEPENDS
    ITKCommon
    ITKPDEDeformableRegistration
    ITKImageIntensity
    ITKSmoothing
    ITKThresholding
    ITKFiniteDifference
    ITKAnisotropicSmoothing
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
