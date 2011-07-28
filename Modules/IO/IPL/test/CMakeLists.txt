itk_module_test()
set(ITKIOIPLTests
itkIOIPLHeaderTest.cxx
)

CreateTestDriver(ITKIOIPL  "${ITKIOIPL-Test_LIBRARIES}" "${ITKIOIPLTests}")

itk_add_test(NAME itkIOIPLHeaderTest
      COMMAND ITKIOIPLTestDriver itkIOIPLHeaderTest)
