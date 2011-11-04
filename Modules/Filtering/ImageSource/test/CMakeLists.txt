itk_module_test()
set(ITKImageSourceTest
  itkGaborImageSourceTest.cxx
)
itk_add_test(NAME itkGaborImageSourceTest0
      COMMAND ITKImageSourceTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/Review/GaborImageSourceTest0.mha}
              ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest0.mha
    itkGaborImageSourceTest ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest0.mha 0)
itk_add_test(NAME itkGaborImageSourceTest1
      COMMAND ITKImageSourceTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/Review/GaborImageSourceTest1.mha}
              ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest1.mha
    itkGaborImageSourceTest ${ITK_TEST_OUTPUT_DIR}/GaborImageSourceTest1.mha 1)
