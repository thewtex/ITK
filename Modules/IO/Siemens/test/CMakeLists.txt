itk_module_test()
set(ITKIOSiemensTests
itkIOSiemensHeaderTest.cxx
)

CreateTestDriver(ITKIOSiemens  "${ITKIOSiemens-Test_LIBRARIES}" "${ITKIOSiemensTests}")

itk_add_test(NAME itkIOSiemensHeaderTest
      COMMAND ITKIOSiemensTestDriver itkIOSiemensHeaderTest)
