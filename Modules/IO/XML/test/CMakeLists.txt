itk_module_test()
set(ITKIOXMLTests
itkIOXMLHeaderTest.cxx
)

CreateTestDriver(ITKIOXML  "${ITKIOXML-Test_LIBRARIES}" "${ITKIOXMLTests}")

itk_add_test(NAME itkIOXMLHeaderTest
      COMMAND ITKIOXMLTestDriver itkIOXMLHeaderTest)
