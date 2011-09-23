itk_module_test()
set(ITKConvolutionTests
  itkConvolutionImageFilterTest.cxx
  itkConvolutionImageFilterTestInt.cxx
  itkConvolutionImageFilterDeltaFunctionTest.cxx
  itkMaskedFFTNormalizedCorrelationImageFilterTest.cxx
)

CreateTestDriver(ITKConvolution  "${ITKConvolution-Test_LIBRARIES}" "${ITKConvolutionTests}")

itk_add_test(NAME itkConvolutionImageFilterTestSobelX
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelX.nii.gz}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nii.gz
    itkConvolutionImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/sobel_x.nii.gz ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nii.gz)
itk_add_test(NAME itkConvolutionImageFilterTestSobelY
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelY.nii.gz}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nii.gz
    itkConvolutionImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/sobel_y.nii.gz ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nii.gz)
itk_add_test(NAME itkConvolutionImageFilterTest4x4Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4Mean.png
    itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest4x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5Mean.png
    itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest5x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png
    itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/5x5-constant.png ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest4x4MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4MeanValidRegion.png
      itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterTest4x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5MeanValidRegion.png
      itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterTest5x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5MeanValidRegion.png
      itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/5x5-constant.png ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterDeltaFunctionTest
      COMMAND ITKConvolutionTestDriver
   --compare ${ITK_DATA_ROOT}/Input/level.png
             ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterDeltaFunctionTest.png
      itkConvolutionImageFilterDeltaFunctionTest ${ITK_DATA_ROOT}/Input/level.png ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterDeltaFunctionTest.png)

# Masked FFT NCC tests
itk_add_test(NAME itkMaskedFFTNormalizedCorrelationImageFilterTest1
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkMaskedFFTNormalizedCorrelationImageFilterTest1.png}
              ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest1.png
    itkMaskedFFTNormalizedCorrelationImageFilterTest DATA{Input/FixedRectangles.png} DATA{Input/MovingRectangles.png} DATA{Input/FixedRectangleMask1.png} DATA{Input/MovingRectanglesMask.png} ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest1.png)
itk_add_test(NAME itkMaskedFFTNormalizedCorrelationImageFilterTest2
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkMaskedFFTNormalizedCorrelationImageFilterTest2.png}
              ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest2.png
    itkMaskedFFTNormalizedCorrelationImageFilterTest DATA{Input/FixedRectangles.png} DATA{Input/MovingRectangles.png} DATA{Input/FixedRectangleMask2.png} DATA{Input/MovingRectanglesMask.png} ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest2.png)
itk_add_test(NAME itkMaskedFFTNormalizedCorrelationImageFilterTest3
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkMaskedFFTNormalizedCorrelationImageFilterTest3.png}
              ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest3.png
    itkMaskedFFTNormalizedCorrelationImageFilterTest DATA{Input/FixedRectangles.png} DATA{Input/MovingRectangles.png} DATA{Input/FixedRectangleMask3.png} DATA{Input/MovingRectanglesMask.png} ${ITK_TEST_OUTPUT_DIR}/itkMaskedFFTNormalizedCorrelationImageFilterTest3.png)
