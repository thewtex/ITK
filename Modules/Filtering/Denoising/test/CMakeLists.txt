itk_module_test()
set(ITKDenoisingTests
itkPatchBasedDenoisingImageFilterTest.cxx
)

CreateTestDriver(ITKDenoising  "${ITKDenoising-Test_LIBRARIES}" "${ITKDenoisingTests}")

itk_add_test(NAME itkPatchBasedDenoisingImageFilterTest0
      COMMAND ITKDenoisingTestDriver
#    --compare DATA{Baseline/itkPatchBasedDenoisingImageFilterTest0.png}
#              ${ITK_TEST_OUTPUT_DIR}/itkPatchBasedDenoisingImageFilterTest0.png
    itkPatchBasedDenoisingImageFilterTest
      ${ITK_DATA_ROOT}/Input/peppers.png
      ${ITK_TEST_OUTPUT_DIR}/itkPatchBasedDenoisingImageFilterTest0.mha
      2 3 5 5)
