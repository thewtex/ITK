itk_module_test()
set(ITK_IO_SpatialObjectsTests
itkPolygonGroupSpatialObjectXMLFileTest.cxx
itkIOSpatialobjectsHeaderTest.cxx
itkReadWriteSpatialObjectTest.cxx
)

CreateTestDriver(ITK_IO_SpatialObjects  "${ITK_IO_SpatialObjects-Test_LIBRARIES}" "${ITK_IO_SpatialObjectsTests}")

itk_add_test(NAME itkIOSpatialobjectsHeaderTest
      COMMAND ITK_IO_SpatialObjectsTestDriver itkIOSpatialobjectsHeaderTest)
itk_add_test(NAME itkReadWriteSpatialObjectTest
      COMMAND ITK_IO_SpatialObjectsTestDriver itkReadWriteSpatialObjectTest
              ${ITK_TEST_OUTPUT_DIR}/Objects.meta)
itk_add_test(NAME itkReadWriteSpatialObjectTest1
      COMMAND ITK_IO_SpatialObjectsTestDriver itkReadWriteSpatialObjectTest
              ${ITK_TEST_OUTPUT_DIR}/Objects1.meta binary)
itk_add_test(NAME itkReadWriteSpatialObjectTest2
      COMMAND ITK_IO_SpatialObjectsTestDriver itkReadWriteSpatialObjectTest
              ${ITK_TEST_OUTPUT_DIR}/Objects2.meta ${ITK_DATA_ROOT}/Input/SpatialObjects.meta)
itk_add_test(NAME itkPolygonGroupSpatialObjectXMLFileTest
      COMMAND ITK_IO_SpatialObjectsTestDriver itkPolygonGroupSpatialObjectXMLFileTest
              ${ITK_TEST_OUTPUT_DIR})
