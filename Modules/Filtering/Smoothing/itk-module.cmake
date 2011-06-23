set(DOCUMENTATION "This module includes the most common image smoothing
filters. For example, Gaussian and Median filters. You may also find it
interesting to look at the ITK_AnisotropicSmoothing group of filters.")

itk_module(ITK_Smoothing
  DEPENDS
    ITK_ImageFilterBase
    ITK_ImageFunction
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
