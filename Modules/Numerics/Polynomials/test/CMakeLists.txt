itk_module_test()
set(ITK_PolynomialsTests
itkMultivariateLegendrePolynomialTest.cxx
itkPolynomialsHeaderTest.cxx
)

CreateTestDriver(ITK_Polynomials  "${ITK_Polynomials-Test_LIBRARIES}" "${ITK_PolynomialsTests}")

itk_add_test(NAME itkPolynomialsHeaderTest
      COMMAND ITK_PolynomialsTestDriver itkPolynomialsHeaderTest)
itk_add_test(NAME itkMultivariateLegendrePolynomialTest
      COMMAND ITK_PolynomialsTestDriver itkMultivariateLegendrePolynomialTest)
