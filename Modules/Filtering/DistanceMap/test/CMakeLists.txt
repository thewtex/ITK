itk_module_test()
set(ITK_DistanceMapTests
itkDanielssonDistanceMapImageFilterTest.cxx
itkContourMeanDistanceImageFilterTest.cxx
itkDistanceMapHeaderTest.cxx
itkContourDirectedMeanDistanceImageFilterTest.cxx
itkFastChamferDistanceImageFilterTest.cxx
itkHausdorffDistanceImageFilterTest.cxx
itkReflectiveImageRegionIteratorTest.cxx
itkSignedMaurerDistanceMapImageFilterTest.cxx
itkSignedDanielssonDistanceMapImageFilterTest.cxx
itkApproximateSignedDistanceMapImageFilterTest.cxx
itkIsoContourDistanceImageFilterTest.cxx
)

CreateTestDriver(ITK_DistanceMap  "${ITK_DistanceMap-Test_LIBRARIES}" "${ITK_DistanceMapTests}")

itk_add_test(NAME itkDistanceMapHeaderTest
      COMMAND ITK_DistanceMapTestDriver itkDistanceMapHeaderTest)
itk_add_test(NAME itkDanielssonDistanceMapImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkDanielssonDistanceMapImageFilterTest)
itk_add_test(NAME itkContourMeanDistanceImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkContourMeanDistanceImageFilterTest)
itk_add_test(NAME itkContourDirectedMeanDistanceImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkContourDirectedMeanDistanceImageFilterTest)
itk_add_test(NAME itkFastChamferDistanceImageFilterTest1
      COMMAND ITK_DistanceMapTestDriver itkFastChamferDistanceImageFilterTest 1)
itk_add_test(NAME itkFastChamferDistanceImageFilterTest2
      COMMAND ITK_DistanceMapTestDriver itkFastChamferDistanceImageFilterTest 2)
itk_add_test(NAME itkFastChamferDistanceImageFilterTest3
      COMMAND ITK_DistanceMapTestDriver itkFastChamferDistanceImageFilterTest 3)
itk_add_test(NAME itkFastChamferDistanceImageFilterTest4
      COMMAND ITK_DistanceMapTestDriver itkFastChamferDistanceImageFilterTest 4)
itk_add_test(NAME itkHausdorffDistanceImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkHausdorffDistanceImageFilterTest)
itk_add_test(NAME itkReflectiveImageRegionIteratorTest
      COMMAND ITK_DistanceMapTestDriver itkReflectiveImageRegionIteratorTest)
itk_add_test(NAME itkSignedMaurerDistanceMapImageFilterTest1
      COMMAND ITK_DistanceMapTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest1.mhd
              ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest1.mhd
    itkSignedMaurerDistanceMapImageFilterTest ${ITK_DATA_ROOT}/Input/SquareBinary201.png ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest1.mhd)
itk_add_test(NAME itkSignedMaurerDistanceMapImageFilterTest2
      COMMAND ITK_DistanceMapTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest2.mhd
              ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest2.mhd
    itkSignedMaurerDistanceMapImageFilterTest ${ITK_DATA_ROOT}/Input/BrainSliceBinary.png ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest2.mhd)
itk_add_test(NAME itkSignedMaurerDistanceMapImageFilterTest3
      COMMAND ITK_DistanceMapTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest3.mhd
              ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest3.mhd
    itkSignedMaurerDistanceMapImageFilterTest ${ITK_DATA_ROOT}/Input/LungSliceBinary.png ${ITK_TEST_OUTPUT_DIR}/itkSignedMaurerDistanceMapImageFilterTest3.mhd)
itk_add_test(NAME itkSignedDanielssonDistanceMapImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkSignedDanielssonDistanceMapImageFilterTest)
itk_add_test(NAME itkApproximateSignedDistanceMapImageFilterTest
      COMMAND ITK_DistanceMapTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkApproximateSignedDistanceMapImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkApproximateSignedDistanceMapImageFilterTest.png
    itkApproximateSignedDistanceMapImageFilterTest ${ITK_TEST_OUTPUT_DIR}/itkApproximateSignedDistanceMapImageFilterTest.png)
itk_add_test(NAME itkIsoContourDistanceImageFilterTest
      COMMAND ITK_DistanceMapTestDriver itkIsoContourDistanceImageFilterTest)
