itk_module_test()
set(ITK_IO_BMPTests
itkBMPImageIOTest2.cxx
itkIOBMPHeaderTest.cxx
itkBMPImageIOTest.cxx
)

CreateTestDriver(ITK_IO_BMP  "${ITK_IO_BMP-Test_LIBRARIES}" "${ITK_IO_BMPTests}")

itk_add_test(NAME itkIOBMPHeaderTest
      COMMAND ITK_IO_BMPTestDriver itkIOBMPHeaderTest)
itk_add_test(NAME itkBMPImageIOTest5
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/smallRGBA.mha
              ${ITK_TEST_OUTPUT_DIR}/smallRGBA.mha
    itkBMPImageIOTest2 ${ITK_DATA_ROOT}/Input/smallRGBA.bmp ${ITK_TEST_OUTPUT_DIR}/smallRGBA.mha)
itk_add_test(NAME itkBMPImageIOTest6
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/smallRGBA.mha
              ${ITK_TEST_OUTPUT_DIR}/smallRGBA.bmp
    itkBMPImageIOTest2 ${ITK_DATA_ROOT}/Input/smallRGBA.bmp ${ITK_TEST_OUTPUT_DIR}/smallRGBA.bmp)
itk_add_test(NAME itkBMPImageIOTest
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/cthead1.bmp
              ${ITK_TEST_OUTPUT_DIR}/cthead1.bmp
    itkBMPImageIOTest ${ITK_DATA_ROOT}/Input/cthead1.bmp ${ITK_TEST_OUTPUT_DIR}/cthead1.bmp)
itk_add_test(NAME itkBMPImageIOTest2
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/cthead254x254.bmp
              ${ITK_TEST_OUTPUT_DIR}/cthead254x254.bmp
    itkBMPImageIOTest ${ITK_DATA_ROOT}/Input/cthead254x254.bmp ${ITK_TEST_OUTPUT_DIR}/cthead254x254.bmp)
itk_add_test(NAME itkBMPImageIOTest3
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/image_grayscale.bmp
              ${ITK_TEST_OUTPUT_DIR}/image_grayscale.bmp
    itkBMPImageIOTest ${ITK_DATA_ROOT}/Input/image_grayscale.bmp ${ITK_TEST_OUTPUT_DIR}/image_grayscale.bmp)
itk_add_test(NAME itkBMPImageIOTest4
      COMMAND ITK_IO_BMPTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/image_color.bmp
              ${ITK_TEST_OUTPUT_DIR}/image_color.bmp
    itkBMPImageIOTest ${ITK_DATA_ROOT}/Input/image_color.bmp ${ITK_TEST_OUTPUT_DIR}/image_color.bmp)
