itk_module_test()
set(ITKConvolutionTests
  itkConvolutionImageFilterTest.cxx
  itkConvolutionImageFilterTestInt.cxx
  itkConvolutionImageFilterDeltaFunctionTest.cxx
  itkFFTConvolutionImageFilterTest.cxx
  itkFFTConvolutionImageFilterTestInt.cxx
  itkFFTConvolutionImageFilterDeltaFunctionTest.cxx
)

CreateTestDriver(ITKConvolution  "${ITKConvolution-Test_LIBRARIES}" "${ITKConvolutionTests}")

itk_add_test(NAME itkConvolutionImageFilterTestSobelX
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelX.nrrd}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nrrd
    itkConvolutionImageFilterTest DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/sobel_x.nii.gz} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nrrd)
itk_add_test(NAME itkConvolutionImageFilterTestSobelY
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelY.nrrd}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nrrd
    itkConvolutionImageFilterTest DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/sobel_y.nii.gz} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nrrd)
itk_add_test(NAME itkConvolutionImageFilterTest4x4Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4Mean.png
    itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest4x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5Mean.png
    itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest5x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png
    itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/5x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png 1 SAME)
itk_add_test(NAME itkConvolutionImageFilterTest4x4MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4MeanValidRegion.png
      itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x4MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterTest4x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5MeanValidRegion.png
      itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest4x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterTest5x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5MeanValidRegion.png
      itkConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/5x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkConvolutionImageFilterDeltaFunctionTest
      COMMAND ITKConvolutionTestDriver
   --compare DATA{${ITK_DATA_ROOT}/Input/level.png}
             ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterDeltaFunctionTest.png
      itkConvolutionImageFilterDeltaFunctionTest DATA{${ITK_DATA_ROOT}/Input/level.png} ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterDeltaFunctionTest.png)

itk_add_test(NAME itkFFTConvolutionImageFilterTestSobelX
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelX.nrrd}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTestSobelX.nrrd
    itkFFTConvolutionImageFilterTest DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/sobel_x.nii.gz} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTestSobelX.nrrd)
itk_add_test(NAME itkFFTConvolutionImageFilterTestSobelY
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTestSobelY.nrrd}
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nrrd
    itkFFTConvolutionImageFilterTest DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/sobel_y.nii.gz} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTestSobelY.nrrd)
itk_add_test(NAME itkFFTConvolutionImageFilterTest4x4Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x4Mean.png
    itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x4Mean.png 1 SAME)
itk_add_test(NAME itkFFTConvolutionImageFilterTest4x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x5Mean.png
    itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x5Mean.png 1 SAME)
itk_add_test(NAME itkFFTConvolutionImageFilterTest5x5Mean
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5Mean.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest5x5Mean.png
    itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/5x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest5x5Mean.png 1 SAME)
itk_add_test(NAME itkFFTConvolutionImageFilterTest4x4MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x4MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x4MeanValidRegion.png
      itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x4-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x4MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkFFTConvolutionImageFilterTest4x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest4x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x5MeanValidRegion.png
      itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{Input/4x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest4x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkFFTConvolutionImageFilterTest5x5MeanValidRegion
      COMMAND ITKConvolutionTestDriver
    --compare DATA{Baseline/itkConvolutionImageFilterTest5x5MeanValidRegion.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest5x5MeanValidRegion.png
      itkFFTConvolutionImageFilterTestInt DATA{${ITK_DATA_ROOT}/Input/cthead1.png} DATA{${ITK_DATA_ROOT}/Input/5x5-constant.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterTest5x5MeanValidRegion.png 1 VALID)
itk_add_test(NAME itkFFTConvolutionImageFilterDeltaFunctionTest
      COMMAND ITKConvolutionTestDriver
   --compare DATA{${ITK_DATA_ROOT}/Input/level.png}
             ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterDeltaFunctionTest.png
      itkFFTConvolutionImageFilterDeltaFunctionTest DATA{${ITK_DATA_ROOT}/Input/level.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTConvolutionImageFilterDeltaFunctionTest.png)
