itk_module_test()
set(ITK_BiasCorrectionTests
itkBiasCorrectionHeaderTest.cxx
itkCompositeValleyFunctionTest.cxx
itkMRIBiasFieldCorrectionFilterTest.cxx
)

CreateTestDriver(ITK_BiasCorrection  "${ITK_BiasCorrection-Test_LIBRARIES}" "${ITK_BiasCorrectionTests}")

itk_add_test(NAME itkBiasCorrectionHeaderTest
      COMMAND ITK_BiasCorrectionTestDriver itkBiasCorrectionHeaderTest)
itk_add_test(NAME itkCompositeValleyFunctionTest
      COMMAND ITK_BiasCorrectionTestDriver itkCompositeValleyFunctionTest)
itk_add_test(NAME itkMRIBiasFieldCorrectionFilterTest
      COMMAND ITK_BiasCorrectionTestDriver itkMRIBiasFieldCorrectionFilterTest)
