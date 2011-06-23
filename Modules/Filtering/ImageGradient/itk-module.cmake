set(DOCUMENTATION "This module contains filters that compute differential
operations in images. In particular, image gradients, gradient magnitude and
difference of Gaussians.")

itk_module(ITK_ImageGradient
  DEPENDS
    ITK_ImageIntensity
    ITK_Smoothing
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
