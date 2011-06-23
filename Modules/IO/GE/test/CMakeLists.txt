itk_module_test()
set(ITK_IO_GETests
itkGEImageIOTest.cxx
itkIOGEHeaderTest.cxx
)

CreateTestDriver(ITK_IO_GE  "${ITK_IO_GE-Test_LIBRARIES}" "${ITK_IO_GETests}")

itk_add_test(NAME itkIOGEHeaderTest
      COMMAND ITK_IO_GETestDriver itkIOGEHeaderTest)
itk_add_test(NAME itkGE4
      COMMAND ITK_IO_GETestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/19771.002.001.mha
              ${ITK_TEST_OUTPUT_DIR}/19771.002.001.mha
    itkGEImageIOTest ${ITK_TEST_OUTPUT_DIR} true GE4 ${ITK_DATA_ROOT}/Input/test_ge4/19771.002.001 ${ITK_TEST_OUTPUT_DIR}/19771.002.001.mha)
itk_add_test(NAME itkGE5
      COMMAND ITK_IO_GETestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/113766.003.001.mha
              ${ITK_TEST_OUTPUT_DIR}/113766.003.001.mha
    itkGEImageIOTest ${ITK_TEST_OUTPUT_DIR} true GE5 ${ITK_DATA_ROOT}/Input/test_ge5/113766.003.001 ${ITK_TEST_OUTPUT_DIR}/113766.003.001.mha)
itk_add_test(NAME itkGEAdw
      COMMAND ITK_IO_GETestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/I.001.mha
              ${ITK_TEST_OUTPUT_DIR}/I.001.mha
    itkGEImageIOTest ${ITK_TEST_OUTPUT_DIR} true GEAdw ${ITK_DATA_ROOT}/Input/test_geadw/I.001 ${ITK_TEST_OUTPUT_DIR}/I.001.mha)
itk_add_test(NAME itkSiemens
      COMMAND ITK_IO_GETestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/3868-2-100.mha
              ${ITK_TEST_OUTPUT_DIR}/3868-2-100.mha
    itkGEImageIOTest ${ITK_TEST_OUTPUT_DIR} true Siemens ${ITK_DATA_ROOT}/Input/test_siemens/3868-2-100.ima ${ITK_TEST_OUTPUT_DIR}/3868-2-100.mha)
itk_add_test(NAME itkGE4Fail
      COMMAND ITK_IO_GETestDriver itkGEImageIOTest
              ${ITK_TEST_OUTPUT_DIR} false GE4 ${ITK_DATA_ROOT}/Input/test_ge5/113766.003.001)
itk_add_test(NAME itkGE5Fail
      COMMAND ITK_IO_GETestDriver itkGEImageIOTest
              ${ITK_TEST_OUTPUT_DIR} false GE5 ${ITK_DATA_ROOT}/Input/test_ge4/19771.002.001)
itk_add_test(NAME itkGEAdwFail
      COMMAND ITK_IO_GETestDriver itkGEImageIOTest
              ${ITK_TEST_OUTPUT_DIR} false GEAdw ${ITK_DATA_ROOT}/Input/test_siemens/3868-2-100.ima)
itk_add_test(NAME itkSiemensFail
      COMMAND ITK_IO_GETestDriver itkGEImageIOTest
              ${ITK_TEST_OUTPUT_DIR} false Siemens ${ITK_DATA_ROOT}/Input/test_geadw/I.001)
