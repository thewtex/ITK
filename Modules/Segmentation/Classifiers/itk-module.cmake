itk_module(ITK_Classifiers
  DEPENDS
    ITK_ImageGrid
    ITK_Statistics
    ITK_ConnectedComponents
  TEST_DEPENDS
    ITK_TestKernel
    ITK_AnisotropicSmoothing
)

# Extra test dependency on ITK_AnisotropicSmoothing is introduced by itkBayesianClassifierImageFilterTest.
