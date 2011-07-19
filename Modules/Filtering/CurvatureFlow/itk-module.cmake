set(DOCUMENTATION "This module contains filters that implement variations of
Curvature Flow. This is a technique that uses an iterative solution of partial
differential equations to implement image denoising image filtering. These
classes are typically used as edge-preserving smoothing filters. You may also
find the Smoothing and AnisotropicSmoothing modules useful as well.")

itk_module(ITKCurvatureFlow
  DEPENDS
    ITKImageFilterBase
    ITKFiniteDifference
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
