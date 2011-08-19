itk_module_test()
set(ITKLevelSetsv4Tests
itkLevelSetImageBaseTest.cxx
itkWhitakerSparseLevelSetBaseTest.cxx
itkShiSparseLevelSetBaseTest.cxx
itkMalcolmSparseLevelSetBaseTest.cxx
)

CreateTestDriver(ITKLevelSetsv4  "${ITKLevelSetsv4-Test_LIBRARIES}" "${ITKLevelSetsv4Tests}")

add_test(NAME itkLevelSetsv4ImageBaseTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetImageBaseTest)
add_test(NAME itkWhitakerSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkWhitakerSparseLevelSetBaseTest)
add_test(NAME itkShiSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkShiSparseLevelSetBaseTest)
add_test(NAME itkMalcolmSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkMalcolmSparseLevelSetBaseTest)
