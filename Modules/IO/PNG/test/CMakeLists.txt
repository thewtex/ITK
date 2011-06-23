itk_module_test()
set(ITK_IO_PNGTests
itkPNGImageIOTest.cxx
itkIOPNGHeaderTest.cxx
)

CreateTestDriver(ITK_IO_PNG  "${ITK_IO_PNG-Test_LIBRARIES}" "${ITK_IO_PNGTests}")

itk_add_test(NAME itkIOPNGHeaderTest
      COMMAND ITK_IO_PNGTestDriver itkIOPNGHeaderTest)
itk_add_test(NAME itkPNGImageIOTest1
      COMMAND ITK_IO_PNGTestDriver itkPNGImageIOTest
              ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkPNGImageIOTest1.png)
itk_add_test(NAME itkPNGImageIOTest2
      COMMAND ITK_IO_PNGTestDriver itkPNGImageIOTest
              ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png ${ITK_TEST_OUTPUT_DIR}/itkPNGImageIOTest2.png)
