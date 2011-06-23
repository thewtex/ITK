itk_module_test()
set(ITK_IO_JPEGTests
itkIOJPEGHeaderTest.cxx
itkJPEGImageIOTest.cxx
)

CreateTestDriver(ITK_IO_JPEG  "${ITK_IO_JPEG-Test_LIBRARIES}" "${ITK_IO_JPEGTests}")

itk_add_test(NAME itkIOJPEGHeaderTest
      COMMAND ITK_IO_JPEGTestDriver itkIOJPEGHeaderTest)
itk_add_test(NAME itkJPEGImageIOTest
      COMMAND ITK_IO_JPEGTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/cthead1.png
              ${ITK_TEST_OUTPUT_DIR}/cthead1.png
    itkJPEGImageIOTest ${ITK_DATA_ROOT}/Input/cthead1.jpg ${ITK_TEST_OUTPUT_DIR}/cthead1.png)
itk_add_test(NAME itkJPEGImageIOTest2
      COMMAND ITK_IO_JPEGTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/cthead1.jpg
              ${ITK_TEST_OUTPUT_DIR}/cthead1.jpg
    itkJPEGImageIOTest ${ITK_DATA_ROOT}/Input/cthead1.jpg ${ITK_TEST_OUTPUT_DIR}/cthead1.jpg)
