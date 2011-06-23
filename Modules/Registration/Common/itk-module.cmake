itk_module(ITK_RegistrationCommon
  DEPENDS
    ITK_Optimizers
    ITK_ImageIntensity
    ITK_ImageFunction
    ITK_ImageGrid
    ITK_SpatialObjects
    ITK_Smoothing
    ITK_ImageGradient
    ITK_ImageFeature
    ITK_FiniteDifference
  TEST_DEPENDS
    ITK_TestKernel
    ITK_DistanceMap
)

# Extra test dependency on ITK_DistanceMap is introduced by itkPointSetToPointSetRegistrationTest.
