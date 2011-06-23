itk_module_test()
set(ITK_MarkovRandomFieldsClassifiersTests
itkMRFImageFilterTest.cxx
itkMarkovRandomFieldsClassifiersHeaderTest.cxx
itkGibbsTest.cxx
)

CreateTestDriver(ITK_MarkovRandomFieldsClassifiers  "${ITK_MarkovRandomFieldsClassifiers-Test_LIBRARIES}" "${ITK_MarkovRandomFieldsClassifiersTests}")

itk_add_test(NAME itkMarkovRandomFieldsClassifiersHeaderTest
      COMMAND ITK_MarkovRandomFieldsClassifiersTestDriver itkMarkovRandomFieldsClassifiersHeaderTest)
itk_add_test(NAME itkMRFImageFilterTest
      COMMAND ITK_MarkovRandomFieldsClassifiersTestDriver itkMRFImageFilterTest)
itk_add_test(NAME itkGibbsTest
      COMMAND ITK_MarkovRandomFieldsClassifiersTestDriver itkGibbsTest)
