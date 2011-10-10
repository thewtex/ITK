itk_module_test()
set(ITKDenoisingTests
itkPatchBasedDenoisingImageFilterTest.cxx
)

CreateTestDriver(ITKDenoising  "${ITKDenoising-Test_LIBRARIES}" "${ITKDenoisingTests}")

itk_add_test(NAME itkPatchBasedDenoisingImageFilterTest0
      COMMAND ITKDenoisingTestDriver
    --compare DATA{Baseline/PatchBasedDenoisingImageFilterTest0.mha}
              ${ITK_TEST_OUTPUT_DIR}/PatchBasedDenoisingImageFilterTest0.mha
    itkPatchBasedDenoisingImageFilterTest
      DATA{Input/noisy_checkerboard.mha}
      ${ITK_TEST_OUTPUT_DIR}/PatchBasedDenoisingImageFilterTest0.mha
      2 1 10 5)
