itk_module_test()
set(ITK_AntiAliasTests
itkAntiAliasHeaderTest.cxx
itkAntiAliasBinaryImageFilterTest.cxx
)

CreateTestDriver(ITK_AntiAlias  "${ITK_AntiAlias-Test_LIBRARIES}" "${ITK_AntiAliasTests}")

itk_add_test(NAME itkAntiAliasHeaderTest
      COMMAND ITK_AntiAliasTestDriver itkAntiAliasHeaderTest)
itk_add_test(NAME itkAntiAliasBinaryImageFilterTest
      COMMAND ITK_AntiAliasTestDriver itkAntiAliasBinaryImageFilterTest)
