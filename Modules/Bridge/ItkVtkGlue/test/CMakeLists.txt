itk_module_test()
set(ITK-ItkVtkGlueTests
itkVtkMedianFilterTest.cxx
)

CreateTestDriver(ITK-ItkVtkGlue "${ITK-ItkVtkGlue-Test_LIBRARIES}" "${ITK-ItkVtkGlueTests}")

itk_add_test(NAME itkVtkMedianImageFilterTest
      COMMAND ITK-ItkVtkGlueTestDriver
      itkVtkMedianFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png 2)
