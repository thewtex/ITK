set(DOCUMENTATION "This module contains classes that compute image features. In
particular you will find here: Canny edge detection, Sobel, ZeroCrossings,
Hough transform for lines and circles, Hessian filters, Vesselness, and
Fractional anisotropy for tensor images.")

itk_module(ITK_ImageFeature
  DEPENDS
    ITK_ImageIntensity
    ITK_Smoothing
    ITK_ImageGradient
    ITK_SpatialObjects
  TEST_DEPENDS
    ITK_TestKernel
    ITK_Thresholding
  DESCRIPTION
    "${DOCUMENTATION}"
  )
# Extra test dependency on ITK_Thresholding is introduced by itkHoughTransform2DCirclesImageTest.
