itk_module_test()

set(ITKIOXMLTests
  itkDOMTest1.cxx
  itkDOMTest2.cxx
  itkDOMTest3.cxx
)

CreateTestDriver(ITKIOXML  "${ITKIOXML-Test_LIBRARIES}" "${ITKIOXMLTests}")

itk_add_test(NAME itkDOMTest1
  COMMAND ITKIOXMLTestDriver itkDOMTest1
)

itk_add_test(NAME itkDOMTest21
  COMMAND ITKIOXMLTestDriver
  itkDOMTest2 DATA{${ITK_DATA_ROOT}/Input/XML/test.xml} ${ITK_TEST_OUTPUT_DIR}/XML/test21-output.xml
)

itk_add_test(NAME itkDOMTest22
  COMMAND ITKIOXMLTestDriver
  itkDOMTest2 DATA{${ITK_DATA_ROOT}/Input/XML/test-malformed.xml} ${ITK_TEST_OUTPUT_DIR}/XML/test22-output.xml
)

itk_add_test(NAME itkDOMTest3
  COMMAND ITKIOXMLTestDriver
  itkDOMTest3 ${ITK_TEST_OUTPUT_DIR}/XML/test3-output.xml
)
