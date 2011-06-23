itk_module_test()
set(ITK_IO_StimulateTests
itkStimulateImageIOTest.cxx
itkIOStimulateHeaderTest.cxx
itkStimulateImageIOTest2.cxx
)

CreateTestDriver(ITK_IO_Stimulate  "${ITK_IO_Stimulate-Test_LIBRARIES}" "${ITK_IO_StimulateTests}")

itk_add_test(NAME itkIOStimulateHeaderTest
      COMMAND ITK_IO_StimulateTestDriver itkIOStimulateHeaderTest)
itk_add_test(NAME itkStimulateImageIOTest
      COMMAND ITK_IO_StimulateTestDriver itkStimulateImageIOTest
              ${ITK_TEST_OUTPUT_DIR}/test1.spr ${ITK_TEST_OUTPUT_DIR}/test2.spr)
itk_add_test(NAME itkStimulateImageIOTest2
      COMMAND ITK_IO_StimulateTestDriver itkStimulateImageIOTest2
              ${ITK_DATA_ROOT}/Input/BigEndian.spr)
