set(DOCUMENTATION "This module contains classes that compute statistics on
images, or use those statistics to perform image operations. In particular you
will find here: how to calculate image moments, minimum maximum values,
projections, principal components analysis (PCA) for shape modeling,
computation of label statistics and image accumulation.")

itk_module(ITK_ImageStatistics
  DEPENDS
    ITK_ImageFilterBase
    ITK_Statistics
    ITK_SpatialObjects
    ITK_ImageCompose
  TEST_DEPENDS
    ITK_ImageIntensity
    ITK_TestKernel
    ITK_Thresholding
    ITK_ImageLabel
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra test dependency on Thresholding is introduced by itkNormalizedCorrelationImageFilterTest;
# Extra test dependency on ImageLabel is introduced by itkBinaryProjectionImageFilterTest.
