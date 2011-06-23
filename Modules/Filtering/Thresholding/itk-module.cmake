set(DOCUMENTATION "This module contains multiple variations of image
thresholding filters. In addition to the classical binary thresholding, you
will find here the thresholding filters based on the Otsu criterion, both for
single and multiple thresholds.")

itk_module(ITK_Thresholding
  DEPENDS
    ITK_ImageIntensity
    ITK_ImageStatistics
  TEST_DEPENDS
    ITK_TestKernel
    ITK_SignedDistanceFunction
    ITK_ImageLabel
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra test dependency on ITK_SignedDistanceFunction  is introduced by itkBinaryThresholdSpatialFunctionTest.
# Extra test dependency on ITK_Smoothing is introduced by itkBinaryThresholdProjectionImageFilterTest.
