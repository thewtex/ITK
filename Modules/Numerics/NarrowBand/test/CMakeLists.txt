itk_module_test()
set(ITK_NarrowBandTests
itkNarrowBandTest.cxx
itkNarrowBandImageFilterBaseTest.cxx
itkNarrowBandHeaderTest.cxx
)

CreateTestDriver(ITK_NarrowBand  "${ITK_NarrowBand-Test_LIBRARIES}" "${ITK_NarrowBandTests}")

itk_add_test(NAME itkNarrowBandHeaderTest
      COMMAND ITK_NarrowBandTestDriver itkNarrowBandHeaderTest)
itk_add_test(NAME itkNarrowBandTest
      COMMAND ITK_NarrowBandTestDriver itkNarrowBandTest)
itk_add_test(NAME itkNarrowBandImageFilterBaseTest
      COMMAND ITK_NarrowBandTestDriver itkNarrowBandImageFilterBaseTest
              ${ITK_TEST_OUTPUT_DIR}/itkNarrowBandImageFilterBaseTest.png)
