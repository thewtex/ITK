set(DOCUMENTATION "This module contains filters that perform pixel-wise
operations on the intensities of images. In particular you will find here
filters that compute trigonometric operations on pixel values, intensity
rescaling, exponentials, conversions between complex and reals, and filters
that combine multiple images into images of multiple components, as well as
filters that compute single scalar images from images of multiple components.")

itk_module(ITK_ImageIntensity
  DEPENDS
    ITK_ImageFilterBase
    ITK_ImageAdaptors
    ITK_ImageStatistics
    ITK_ImageGrid
    ITK_Path
  TEST_DEPENDS
    ITK_TestKernel
    ITK_SpatialObjects
    ITK_DistanceMap
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra dependency of ITK_SpatialObjects is introduced by itkPolylineMaskImageFilterTest.
# Extra dependency of ITK_SpatialObjects is introduced by itkModulusImageFilterTest.
