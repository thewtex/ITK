itk_module(ITK_KLMRegionGrowing
  DEPENDS
    ITK_Common
  TEST_DEPENDS
    ITK_TestKernel
    ITK_Statistics
)

# Extra test dependency on ITK_Statistics in introduced by itkRegionGrow2DTest.
