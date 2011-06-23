itk_module_test()
set(ITK_IO_LSMTests
itkIOLSMHeaderTest.cxx
itkLSMImageIOTest.cxx
)

CreateTestDriver(ITK_IO_LSM  "${ITK_IO_LSM-Test_LIBRARIES}" "${ITK_IO_LSMTests}")

itk_add_test(NAME itkIOLSMHeaderTest
      COMMAND ITK_IO_LSMTestDriver itkIOLSMHeaderTest)
itk_add_test(NAME itkLSMImageIOTest
      COMMAND ITK_IO_LSMTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/cthead1.tif
              ${ITK_TEST_OUTPUT_DIR}/cthead1.tif
    itkLSMImageIOTest ${ITK_DATA_ROOT}/Input/cthead1.lsm ${ITK_TEST_OUTPUT_DIR}/cthead1.tif)
