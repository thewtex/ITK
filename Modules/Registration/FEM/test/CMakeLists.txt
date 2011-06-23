itk_module_test()
set(ITK_FEMRegistrationTests
itkFEMRegistrationFilterTest.cxx
itkMIRegistrationFunctionTest.cxx
itkFEMRegistrationHeaderTest.cxx
)

CreateTestDriver(ITK_FEMRegistration  "${ITK_FEMRegistration-Test_LIBRARIES}" "${ITK_FEMRegistrationTests}")

itk_add_test(NAME itkFEMRegistrationHeaderTest
      COMMAND ITK_FEMRegistrationTestDriver itkFEMRegistrationHeaderTest)
itk_add_test(NAME itkFEMRegistrationFilterTest
      COMMAND ITK_FEMRegistrationTestDriver itkFEMRegistrationFilterTest)
itk_add_test(NAME itkMIRegistrationFunctionTest
      COMMAND ITK_FEMRegistrationTestDriver itkMIRegistrationFunctionTest)
