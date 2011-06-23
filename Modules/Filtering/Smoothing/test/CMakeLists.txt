itk_module_test()
set(ITK_SmoothingTests
itkDiscreteGaussianImageFilterTest2.cxx
itkSmoothingRecursiveGaussianImageFilterTest.cxx
itkMeanImageFilterTest.cxx
itkDiscreteGaussianImageFilterTest.cxx
itkMedianImageFilterTest.cxx
itkSmoothingHeaderTest.cxx
itkRecursiveGaussianImageFiltersOnTensorsTest.cxx
itkRecursiveGaussianImageFiltersOnVectorImageTest.cxx
itkRecursiveGaussianImageFiltersTest.cxx
itkRecursiveGaussianScaleSpaceTest1.cxx
)

CreateTestDriver(ITK_Smoothing  "${ITK_Smoothing-Test_LIBRARIES}" "${ITK_SmoothingTests}")

itk_add_test(NAME itkSmoothingHeaderTest
      COMMAND ITK_SmoothingTestDriver itkSmoothingHeaderTest)
itk_add_test(NAME itkDiscreteGaussianImageFilterTest2
      COMMAND ITK_SmoothingTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/DiscreteGaussianImageFilterTest2_OutputA.mha
              ${ITK_TEST_OUTPUT_DIR}/DiscreteGaussianImageFilterTest2_OutputA.mha
    itkDiscreteGaussianImageFilterTest2 2 3 ${ITK_DATA_ROOT}/Input/RGBTestImage.tif 3.5 ${ITK_TEST_OUTPUT_DIR}/DiscreteGaussianImageFilterTest2_OutputA.mha)
itk_add_test(NAME itkSmoothingRecursiveGaussianImageFilterTest
      COMMAND ITK_SmoothingTestDriver itkSmoothingRecursiveGaussianImageFilterTest)
itk_add_test(NAME itkMeanImageFilterTest
      COMMAND ITK_SmoothingTestDriver itkMeanImageFilterTest)
itk_add_test(NAME itkDiscreteGaussianImageFilterTest
      COMMAND ITK_SmoothingTestDriver itkDiscreteGaussianImageFilterTest)
itk_add_test(NAME itkMedianImageFilterTest
      COMMAND ITK_SmoothingTestDriver itkMedianImageFilterTest)
itk_add_test(NAME itkRecursiveGaussianImageFiltersOnTensorsTest
      COMMAND ITK_SmoothingTestDriver itkRecursiveGaussianImageFiltersOnTensorsTest)
itk_add_test(NAME itkRecursiveGaussianImageFiltersOnVectorImageTest
      COMMAND ITK_SmoothingTestDriver itkRecursiveGaussianImageFiltersOnVectorImageTest)
itk_add_test(NAME itkRecursiveGaussianImageFiltersTest
      COMMAND ITK_SmoothingTestDriver itkRecursiveGaussianImageFiltersTest)
itk_add_test(NAME itkRecursiveGaussianScaleSpaceTest1
      COMMAND ITK_SmoothingTestDriver
              itkRecursiveGaussianScaleSpaceTest1)
