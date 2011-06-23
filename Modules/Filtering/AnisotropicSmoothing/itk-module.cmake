set(DOCUMENTATION "This module contains filters that implement variations of
anisotropic smoothing. This is an image denoising technique that strives for
preserving edges on the images while smoothing regions of uniform intensity.
This type of filtering is convenient as a preprocessing stage of segmentation
algorithms. You may find useful as well the filters in the ITK_CurvatureFlow
module and the ITK_SmoothingModule.")

itk_module(ITK_AnisotropicSmoothing
  DEPENDS
    ITK_ImageGrid
    ITK_CurvatureFlow
    ITK_FiniteDifference
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
