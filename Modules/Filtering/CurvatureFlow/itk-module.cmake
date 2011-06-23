set(DOCUMENTATION "This module contains filters that implement variations of
Curvature Flow. This is a technique that uses an iterative solution of partial
differential equations to implement image denoising image filtering. These
classes are typically used as edge-preserving smoothing filters. You may find
useful as well to look at the Smoothing and ITK_AnisotropicSmoothing modules.")

itk_module(ITK_CurvatureFlow
  DEPENDS
    ITK_ImageFilterBase
    ITK_FiniteDifference
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
