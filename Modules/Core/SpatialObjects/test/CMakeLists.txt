itk_module_test()
set(
  ITKSpatialObjectsTests
  itkCastSpatialObjectFilterTest.cxx
  itkContourSpatialObjectPointTest.cxx
  itkSpatialObjectToImageFilterTest.cxx
  itkLandmarkSpatialObjectTest.cxx
  itkImageSpatialObjectTest.cxx
  itkSpatialObjectToImageStatisticsCalculatorTest.cxx
  itkImageMaskSpatialObjectTest.cxx
  itkImageMaskSpatialObjectTest2.cxx
  itkImageMaskSpatialObjectTest3.cxx
  itkImageMaskSpatialObjectTest4.cxx
  itkImageMaskSpatialObjectTest5.cxx
  itkBlobSpatialObjectTest.cxx
  itkContourSpatialObjectTest.cxx
  itkSurfaceSpatialObjectTest.cxx
  itkMetaArrowConverterTest.cxx
  itkMetaGaussianConverterTest.cxx
  itkTubeSpatialObjectTest.cxx
  itkDTITubeSpatialObjectTest.cxx
  itkSpatialObjectToPointSetFilterTest.cxx
  itkSpatialObjectDuplicatorTest.cxx
  itkBoxSpatialObjectTest.cxx
  itkGaussianSpatialObjectTest.cxx
  itkPolygonSpatialObjectTest.cxx
  itkPolygonSpatialObjectIsInsideInObjectSpaceTest.cxx
  itkEllipseSpatialObjectTest.cxx
  itkMeshSpatialObjectTest.cxx
  itkArrowSpatialObjectTest.cxx
  itkLineSpatialObjectTest.cxx
  itkNewMetaObjectTypeTest.cxx
)

createtestdriver(ITKSpatialObjects "${ITKSpatialObjects-Test_LIBRARIES}" "${ITKSpatialObjectsTests}")

itk_add_test(
  NAME itkCastSpatialObjectFilterTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkCastSpatialObjectFilterTest
)
itk_add_test(
  NAME itkContourSpatialObjectPointTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkContourSpatialObjectPointTest
)
itk_add_test(
  NAME itkSpatialObjectToImageFilterTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkSpatialObjectToImageFilterTest
)
itk_add_test(
  NAME itkLandmarkSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkLandmarkSpatialObjectTest
)
itk_add_test(
  NAME itkImageSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageSpatialObjectTest
)
itk_add_test(
  NAME itkSpatialObjectToImageStatisticsCalculatorTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkSpatialObjectToImageStatisticsCalculatorTest
)
itk_add_test(
  NAME itkImageMaskSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageMaskSpatialObjectTest
)
itk_add_test(
  NAME itkImageMaskSpatialObjectTest2
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageMaskSpatialObjectTest2
)
itk_add_test(
  NAME itkImageMaskSpatialObjectTest3
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageMaskSpatialObjectTest3
)
itk_add_test(
  NAME itkImageMaskSpatialObjectTest4
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageMaskSpatialObjectTest4
)
itk_add_test(
  NAME itkImageMaskSpatialObjectTest5
  COMMAND
    ITKSpatialObjectsTestDriver
    itkImageMaskSpatialObjectTest5
)
itk_add_test(
  NAME itkBlobSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkBlobSpatialObjectTest
)
itk_add_test(
  NAME itkContourSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkContourSpatialObjectTest
)
itk_add_test(
  NAME itkSurfaceSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkSurfaceSpatialObjectTest
)
itk_add_test(
  NAME itkMetaArrowConverterTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkMetaArrowConverterTest
    ${ITK_TEST_OUTPUT_DIR}/MetaArrowConverterTestFile.mha
)
itk_add_test(
  NAME itkMetaGaussianConverterTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkMetaGaussianConverterTest
    ${ITK_TEST_OUTPUT_DIR}/MetaGaussianConverterTest.mha
)
itk_add_test(
  NAME itkTubeSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkTubeSpatialObjectTest
)
itk_add_test(
  NAME itkDTITubeSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkDTITubeSpatialObjectTest
)
itk_add_test(
  NAME itkSpatialObjectToPointSetFilterTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkSpatialObjectToPointSetFilterTest
)
itk_add_test(
  NAME itkSpatialObjectDuplicatorTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkSpatialObjectDuplicatorTest
)
itk_add_test(
  NAME itkBoxSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    --compare
    DATA{${ITK_DATA_ROOT}/Baseline/SpatialObjects/BoxSpatialObjectTest.png}
    ${ITK_TEST_OUTPUT_DIR}/BoxSpatialObjectTest.png
    itkBoxSpatialObjectTest
    ${ITK_TEST_OUTPUT_DIR}/BoxSpatialObjectTest.png
)
itk_add_test(
  NAME itkGaussianSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkGaussianSpatialObjectTest
)
itk_add_test(
  NAME itkPolygonSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkPolygonSpatialObjectTest
)
itk_add_test(
  NAME itkPolygonSpatialObjectIsInsideInObjectSpaceTest
  COMMAND
    ITKSpatialObjectsTestDriver
    --compareIntensityTolerance
    0
    --compare
    DATA{Baseline/itkPolygonSpatialObjectIsInsideInObjectSpaceTest.mha}
    ${ITK_TEST_OUTPUT_DIR}/itkPolygonSpatialObjectIsInsideInObjectSpaceTest.mha
    itkPolygonSpatialObjectIsInsideInObjectSpaceTest
    DATA{Input/PolygonContourPoints.csv}
    DATA{Baseline/itkPolygonSpatialObjectIsInsideInObjectSpaceTest.mha}
    ${ITK_TEST_OUTPUT_DIR}/itkPolygonSpatialObjectIsInsideInObjectSpaceTest.mha
)
itk_add_test(
  NAME itkEllipseSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkEllipseSpatialObjectTest
)
itk_add_test(
  NAME itkMeshSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkMeshSpatialObjectTest
)
itk_add_test(
  NAME itkArrowSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkArrowSpatialObjectTest
)
itk_add_test(
  NAME itkLineSpatialObjectTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkLineSpatialObjectTest
)
itk_add_test(
  NAME itkNewMetaObjectTypeTest
  COMMAND
    ITKSpatialObjectsTestDriver
    itkNewMetaObjectTypeTest
    ${ITK_TEST_OUTPUT_DIR}/NewMetaObjectType.meta
)

set(
  ITKSpatialObjectsGTests
  itkImageMaskSpatialObjectGTest.cxx
  itkSpatialObjectPointGTest.cxx
)
creategoogletestdriver(ITKSpatialObjects "${ITKSpatialObjects-Test_LIBRARIES}" "${ITKSpatialObjectsGTests}")
