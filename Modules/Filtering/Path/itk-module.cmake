  set(DOCUMENTATION "The classes in this module are intendeded to describe the
concept of a linear path in N-Dimensional space. They can be used to represent
contours in 2D images, or curves in 3D space. These classes also include the
concept of iterators, polylines, and smooth approximations to paths.")

itk_module(ITK_Path
  DEPENDS
    ITK_ImageFilterBase
    ITK_ImageFunction
  TEST_DEPENDS
    ITK_TestKernel
    ITK_ImageIntensity
    ITK_Smoothing
    ITK_ImageFeature
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra test dependencies on ITK_ImageIntensity, ITK_Smoothing, and ITK_ImageFeature are introduced by itkOrthogonalSwath2DPathFilterTest.
