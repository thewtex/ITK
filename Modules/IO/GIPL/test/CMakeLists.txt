itk_module_test()
set(ITK_IO_GIPLTests
itkIOGIPLHeaderTest.cxx
itkGiplImageIOTest.cxx
)

CreateTestDriver(ITK_IO_GIPL  "${ITK_IO_GIPL-Test_LIBRARIES}" "${ITK_IO_GIPLTests}")

itk_add_test(NAME itkIOGIPLHeaderTest
      COMMAND ITK_IO_GIPLTestDriver itkIOGIPLHeaderTest)
itk_add_test(NAME itkGiplImageIOTest
      COMMAND ITK_IO_GIPLTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/ramp.mhd
              ${ITK_TEST_OUTPUT_DIR}/ramp.mhd
    itkGiplImageIOTest ${ITK_DATA_ROOT}/Input/ramp.gipl ${ITK_TEST_OUTPUT_DIR}/ramp.mhd)
itk_add_test(NAME itkGiplImageIOGzTest
      COMMAND ITK_IO_GIPLTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/ramp.mhd
              ${ITK_TEST_OUTPUT_DIR}/ramp-gz.mhd
    itkGiplImageIOTest ${ITK_DATA_ROOT}/Input/ramp.gipl.gz ${ITK_TEST_OUTPUT_DIR}/ramp-gz.mhd)
itk_add_test(NAME itkGiplImageIOTest2
      COMMAND ITK_IO_GIPLTestDriver itkGiplImageIOTest
              ${ITK_DATA_ROOT}/Input/ramp.gipl ${ITK_TEST_OUTPUT_DIR}/ramp.gipl)
