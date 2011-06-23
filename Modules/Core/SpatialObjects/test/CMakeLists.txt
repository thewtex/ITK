itk_module_test()
set(ITK_SpatialObjectsTests
itkSpatialObjectToImageFilterTest.cxx
itkLandmarkSpatialObjectTest.cxx
itkSpatialObjectsHeaderTest.cxx
itkPolygonGroupSpatialObjectTest.cxx
itkImageSpatialObjectTest.cxx
itkSpatialObjectToImageStatisticsCalculatorTest.cxx
itkImageMaskSpatialObjectTest.cxx
itkImageMaskSpatialObjectTest2.cxx
itkImageMaskSpatialObjectTest3.cxx
itkBlobSpatialObjectTest.cxx
itkContourSpatialObjectTest.cxx
itkSceneSpatialObjectTest.cxx
itkSurfaceSpatialObjectTest.cxx
itkMetaArrowConverterTest.cxx
itkTubeSpatialObjectTest.cxx
itkSpatialObjectToPointSetFilterTest.cxx
itkSpatialObjectDuplicatorTest.cxx
itkPlaneSpatialObjectTest.cxx
itkCylinderSpatialObjectTest.cxx
itkSpatialObjectTreeContainerTest.cxx
itkAffineGeometryFrameTest.cxx
itkBoxSpatialObjectTest.cxx
itkGaussianSpatialObjectTest.cxx
itkPolygonSpatialObjectTest.cxx
itkEllipseSpatialObjectTest.cxx
itkMeshSpatialObjectTest.cxx
itkArrowSpatialObjectTest.cxx
itkLineSpatialObjectTest.cxx
)

CreateTestDriver(ITK_SpatialObjects  "${ITK_SpatialObjects-Test_LIBRARIES}" "${ITK_SpatialObjectsTests}")

itk_add_test(NAME itkSpatialObjectsHeaderTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectsHeaderTest)
itk_add_test(NAME itkSpatialObjectToImageFilterTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectToImageFilterTest)
itk_add_test(NAME itkLandmarkSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkLandmarkSpatialObjectTest)
itk_add_test(NAME itkPolygonGroupSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkPolygonGroupSpatialObjectTest)
itk_add_test(NAME itkImageSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkImageSpatialObjectTest)
itk_add_test(NAME itkSpatialObjectToImageStatisticsCalculatorTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectToImageStatisticsCalculatorTest)
itk_add_test(NAME itkImageMaskSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkImageMaskSpatialObjectTest)
itk_add_test(NAME itkImageMaskSpatialObjectTest2
      COMMAND ITK_SpatialObjectsTestDriver itkImageMaskSpatialObjectTest2)
itk_add_test(NAME itkImageMaskSpatialObjectTest3
      COMMAND ITK_SpatialObjectsTestDriver itkImageMaskSpatialObjectTest3)
itk_add_test(NAME itkBlobSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkBlobSpatialObjectTest)
itk_add_test(NAME itkContourSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkContourSpatialObjectTest)
itk_add_test(NAME itkSceneSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkSceneSpatialObjectTest)
itk_add_test(NAME itkSurfaceSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkSurfaceSpatialObjectTest)
itk_add_test(NAME itkMetaArrowConverterTest
      COMMAND ITK_SpatialObjectsTestDriver itkMetaArrowConverterTest
              ${ITK_TEST_OUTPUT_DIR}/MetaArrowConverterTestFile.mha)
itk_add_test(NAME itkTubeSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkTubeSpatialObjectTest)
itk_add_test(NAME itkSpatialObjectToPointSetFilterTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectToPointSetFilterTest)
itk_add_test(NAME itkSpatialObjectDuplicatorTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectDuplicatorTest)
itk_add_test(NAME itkPlaneSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkPlaneSpatialObjectTest)
itk_add_test(NAME itkCylinderSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkCylinderSpatialObjectTest)
itk_add_test(NAME itkSpatialObjectTreeContainerTest
      COMMAND ITK_SpatialObjectsTestDriver itkSpatialObjectTreeContainerTest)
itk_add_test(NAME itkAffineGeometryFrameTest
      COMMAND ITK_SpatialObjectsTestDriver itkAffineGeometryFrameTest)
itk_add_test(NAME itkBoxSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/SpatialObjects/BoxSpatialObjectTest.png
              ${ITK_TEST_OUTPUT_DIR}/BoxSpatialObjectTest.png
    itkBoxSpatialObjectTest ${ITK_TEST_OUTPUT_DIR}/BoxSpatialObjectTest.png)
itk_add_test(NAME itkGaussianSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkGaussianSpatialObjectTest)
itk_add_test(NAME itkPolygonSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkPolygonSpatialObjectTest)
itk_add_test(NAME itkEllipseSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkEllipseSpatialObjectTest)
itk_add_test(NAME itkMeshSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkMeshSpatialObjectTest)
itk_add_test(NAME itkArrowSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkArrowSpatialObjectTest)
itk_add_test(NAME itkLineSpatialObjectTest
      COMMAND ITK_SpatialObjectsTestDriver itkLineSpatialObjectTest)
