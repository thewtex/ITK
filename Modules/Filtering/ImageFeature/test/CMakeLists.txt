itk_module_test()
set(ITK_ImageFeatureTests
itkZeroCrossingBasedEdgeDetectionImageFilterTest.cxx
itkLaplacianImageFilterTest.cxx
itkSobelEdgeDetectionImageFilterTest.cxx
itkHessian3DToVesselnessMeasureImageFilterTest.cxx
itkHessianRecursiveGaussianFilterScaleSpaceTest.cxx
itkHessianRecursiveGaussianFilterTest.cxx
itkHoughTransform2DCirclesImageTest.cxx
itkHoughTransform2DLinesImageTest.cxx
itkCannyEdgeDetectionImageFilterTest.cxx
itkBilateralImageFilterTest.cxx
itkBilateralImageFilterTest2.cxx
itkBilateralImageFilterTest3.cxx
itkGradientVectorFlowImageFilterTest.cxx
itkImageFeatureHeaderTest.cxx
itkSimpleContourExtractorImageFilterTest.cxx
itkZeroCrossingImageFilterTest.cxx
itkCannyEdgeDetectionImageFilterTest2.cxx
itkDerivativeImageFilterTest.cxx
itkLaplacianRecursiveGaussianImageFilterTest.cxx
)

CreateTestDriver(ITK_ImageFeature  "${ITK_ImageFeature-Test_LIBRARIES}" "${ITK_ImageFeatureTests}")

itk_add_test(NAME itkImageFeatureHeaderTest
      COMMAND ITK_ImageFeatureTestDriver itkImageFeatureHeaderTest)
itk_add_test(NAME itkZeroCrossingBasedEdgeDetectionImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkZeroCrossingBasedEdgeDetectionImageFilterTest)
itk_add_test(NAME itkLaplacianImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkLaplacianImageFilterTest)
itk_add_test(NAME itkSobelEdgeDetectionImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkSobelEdgeDetectionImageFilterTest)
itk_add_test(NAME itkHessian3DToVesselnessMeasureImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkHessian3DToVesselnessMeasureImageFilterTest)
itk_add_test(NAME itkHessianRecursiveGaussianFilterScaleSpaceTest
      COMMAND ITK_ImageFeatureTestDriver itkHessianRecursiveGaussianFilterScaleSpaceTest)
itk_add_test(NAME itkHessianRecursiveGaussianFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkHessianRecursiveGaussianFilterTest)
itk_add_test(NAME itkHoughTransform2DCirclesImageTest
      COMMAND ITK_ImageFeatureTestDriver itkHoughTransform2DCirclesImageTest)
itk_add_test(NAME itkHoughTransform2DLinesImageTest
      COMMAND ITK_ImageFeatureTestDriver itkHoughTransform2DLinesImageTest)
itk_add_test(NAME itkCannyEdgeDetectionImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkCannyEdgeDetectionImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest.png
    itkCannyEdgeDetectionImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest.png)
itk_add_test(NAME itkBilateralImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkBilateralImageFilterTest)
itk_add_test(NAME itkBilateralImageFilterTest2
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/BilateralImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/BilateralImageFilterTest2.png
    itkBilateralImageFilterTest2 ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_TEST_OUTPUT_DIR}/BilateralImageFilterTest2.png)
itk_add_test(NAME itkBilateralImageFilterTest3
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/BilateralImageFilterTest3.png
              ${ITK_TEST_OUTPUT_DIR}/BilateralImageFilterTest3.png
    itkBilateralImageFilterTest3 ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_TEST_OUTPUT_DIR}/BilateralImageFilterTest3.png)
itk_add_test(NAME itkGradientVectorFlowImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkGradientVectorFlowImageFilterTest)
itk_add_test(NAME itkSimpleContourExtractorImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/SimpleContourExtractorImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/SimpleContourExtractorImageFilterTest.png
    itkSimpleContourExtractorImageFilterTest ${ITK_DATA_ROOT}/Baseline/BasicFilters/BinaryThresholdImageFilterTest2.png ${ITK_TEST_OUTPUT_DIR}/SimpleContourExtractorImageFilterTest.png)
itk_add_test(NAME itkZeroCrossingImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver itkZeroCrossingImageFilterTest)
itk_add_test(NAME itkCannyEdgeDetectionImageFilterTest2
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest2_A.png
              ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest2_B.png
    itkCannyEdgeDetectionImageFilterTest2 ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest2_A.png ${ITK_TEST_OUTPUT_DIR}/itkCannyEdgeDetectionImageFilterTest2_B.png)
itk_add_test(NAME itkDerivativeImageFilterTest1x
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkDerivativeImageFilterTest1x.png
              ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest1x.png
    itkDerivativeImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest1x.png 1 0)
itk_add_test(NAME itkDerivativeImageFilterTest1y
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkDerivativeImageFilterTest1y.png
              ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest1y.png
    itkDerivativeImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest1y.png 1 1)
itk_add_test(NAME itkDerivativeImageFilterTest2x
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkDerivativeImageFilterTest2x.png
              ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest2x.png
    itkDerivativeImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest2x.png 2 0)
itk_add_test(NAME itkDerivativeImageFilterTest2y
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkDerivativeImageFilterTest2y.png
              ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest2y.png
    itkDerivativeImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkDerivativeImageFilterTest2y.png 2 1)
itk_add_test(NAME itkLaplacianRecursiveGaussianImageFilterTest
      COMMAND ITK_ImageFeatureTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/LaplacianRecursiveGaussianImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/LaplacianRecursiveGaussianImageFilterTest.png
    itkLaplacianRecursiveGaussianImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/LaplacianRecursiveGaussianImageFilterTest.png)
