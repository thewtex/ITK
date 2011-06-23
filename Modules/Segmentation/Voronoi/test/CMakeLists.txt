itk_module_test()
set(ITK_VoronoiTests
itkVoronoiHeaderTest.cxx
itkVoronoiSegmentationImageFilterTest.cxx
itkVoronoiSegmentationRGBImageFilterTest.cxx
itkVoronoiDiagram2DTest.cxx
itkVoronoiPartitioningImageFilterTest.cxx
)

CreateTestDriver(ITK_Voronoi  "${ITK_Voronoi-Test_LIBRARIES}" "${ITK_VoronoiTests}")

itk_add_test(NAME itkVoronoiHeaderTest
      COMMAND ITK_VoronoiTestDriver itkVoronoiHeaderTest)
itk_add_test(NAME itkVoronoiSegmentationImageFilterTest
      COMMAND ITK_VoronoiTestDriver itkVoronoiSegmentationImageFilterTest)
itk_add_test(NAME itkVoronoiSegmentationRGBImageFilterTest
      COMMAND ITK_VoronoiTestDriver itkVoronoiSegmentationRGBImageFilterTest)
itk_add_test(NAME itkVoronoiDiagram2DTest
      COMMAND ITK_VoronoiTestDriver itkVoronoiDiagram2DTest)
itk_add_test(NAME itkVoronoiPartitioningImageFilterTest1
      COMMAND ITK_VoronoiTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.png
              ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest1.png
    itkVoronoiPartitioningImageFilterTest ${ITK_DATA_ROOT}/Input/sf4.png ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest1.png 1)
itk_add_test(NAME itkVoronoiPartitioningImageFilterTest2
      COMMAND ITK_VoronoiTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/VoronoiPartioningImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest2.png
    itkVoronoiPartitioningImageFilterTest ${ITK_DATA_ROOT}/Input/sf4.png ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest2.png 0)
