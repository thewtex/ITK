itk_module_test()
set(ITK-ConvolutionTests
itkConvolutionImageFilterTest.cxx
itkConvolutionImageFilterTestInt.cxx
)

set(BASELINE ${ITK_DATA_ROOT}/Baseline/Review)
set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITK-Convolution  "${ITK-Convolution-Test_LIBRARIES}" "${ITK-ConvolutionTests}")

itk_add_test(NAME itkConvolutionImageFilterTestSobelX
      COMMAND ITK-ConvolutionTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Review/itkConvolutionImageFilterTestSobelX.nii.gz
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nii.gz
    itkConvolutionImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/sobel_x.nii.gz ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelX.nii.gz)
itk_add_test(NAME itkConvolutionImageFilterTestSobelY
      COMMAND ITK-ConvolutionTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Review/itkConvolutionImageFilterTestSobelY.nii.gz
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nii.gz
    itkConvolutionImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/sobel_y.nii.gz ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTestSobelY.nii.gz)
itk_add_test(NAME itkConvolutionImageFilterTest5x5Mean
      COMMAND ITK-ConvolutionTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Review/itkConvolutionImageFilterTest5x5Mean.png
              ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png
    itkConvolutionImageFilterTestInt ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/5x5-constant.png ${ITK_TEST_OUTPUT_DIR}/itkConvolutionImageFilterTest5x5Mean.png 1)
