itk_module_test()
set(ITK_EigenTests
itkEigenHeaderTest.cxx
itkEigenAnalysis2DImageFilterTest.cxx
itkSymmetricEigenSystemTest.cxx
)

CreateTestDriver(ITK_Eigen  "${ITK_Eigen-Test_LIBRARIES}" "${ITK_EigenTests}")

itk_add_test(NAME itkEigenHeaderTest
      COMMAND ITK_EigenTestDriver itkEigenHeaderTest)
itk_add_test(NAME itkEigenAnalysis2DImageFilterTest
      COMMAND ITK_EigenTestDriver itkEigenAnalysis2DImageFilterTest)
itk_add_test(NAME itkSymmetricEigenSystemTest
      COMMAND ITK_EigenTestDriver itkSymmetricEigenSystemTest)
