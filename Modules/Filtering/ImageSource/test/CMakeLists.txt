itk_module_test()
set(ITKImageSourceTests
  itkGaborImageSourceTest.cxx
  itkGaussianImageSourceTest.cxx
)

CreateTestDriver(ITKImageSource "${ITKImageSource-Test_LIBRARIES}" "${ITKImageSourceTests}")

itk_add_test(NAME itkGaborImageSourceTest0
      COMMAND ITKImageSourceTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/Filtering/GaborImageSourceTest0.mha}
              ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest0.mha
    itkGaborImageSourceTest ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest0.mha 0)
itk_add_test(NAME itkGaborImageSourceTest1
      COMMAND ITKImageSourceTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/Filtering/GaborImageSourceTest1.mha}
              ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest1.mha
    itkGaborImageSourceTest ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest1.mha 1)
itk_add_test(NAME itkGaussianImageSourceTest
      COMMAND ITKImageSourceTestDriver itkGaussianImageSourceTest)
