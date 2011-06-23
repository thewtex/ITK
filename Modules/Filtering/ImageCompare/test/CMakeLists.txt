itk_module_test()
set(ITK_ImageCompareTests
itkImageCompareHeaderTest.cxx
itkAbsoluteValueDifferenceImageFilterTest.cxx
itkSquaredDifferenceImageFilterTest.cxx
itkCheckerBoardImageFilterTest.cxx
itkConstrainedValueDifferenceImageFilterTest.cxx
itkSimilarityIndexImageFilterTest.cxx
itkSTAPLEImageFilterTest.cxx
itkDifferenceImageFilterTest.cxx
)

CreateTestDriver(ITK_ImageCompare  "${ITK_ImageCompare-Test_LIBRARIES}" "${ITK_ImageCompareTests}")

itk_add_test(NAME itkImageCompareHeaderTest
      COMMAND ITK_ImageCompareTestDriver itkImageCompareHeaderTest)
itk_add_test(NAME itkAbsoluteValueDifferenceImageFilterTest
      COMMAND ITK_ImageCompareTestDriver itkAbsoluteValueDifferenceImageFilterTest)
itk_add_test(NAME itkSquaredDifferenceImageFilterTest
      COMMAND ITK_ImageCompareTestDriver itkSquaredDifferenceImageFilterTest)
itk_add_test(NAME itkCheckerBoardImageFilterTest
      COMMAND ITK_ImageCompareTestDriver itkCheckerBoardImageFilterTest)
itk_add_test(NAME itkConstrainedValueDifferenceImageFilterTest
      COMMAND ITK_ImageCompareTestDriver itkConstrainedValueDifferenceImageFilterTest)
itk_add_test(NAME itkSimilarityIndexImageFilterTest
      COMMAND ITK_ImageCompareTestDriver itkSimilarityIndexImageFilterTest)
itk_add_test(NAME itkSTAPLEImageFilterTest
      COMMAND ITK_ImageCompareTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/STAPLEImageFilterTest.mha
              ${ITK_TEST_OUTPUT_DIR}/STAPLEImageFilterTest.mha
    itkSTAPLEImageFilterTest 2 ${ITK_TEST_OUTPUT_DIR}/STAPLEImageFilterTest.mha 255 0.5 ${ITK_DATA_ROOT}/Input/STAPLE1.png ${ITK_DATA_ROOT}/Input/STAPLE2.png ${ITK_DATA_ROOT}/Input/STAPLE3.png ${ITK_DATA_ROOT}/Input/STAPLE4.png)
itk_add_test(NAME itkDifferenceImageFilterTest
      COMMAND ITK_ImageCompareTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Common/itkDifferenceImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkDifferenceImageFilterTest.png
    itkDifferenceImageFilterTest ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_DATA_ROOT}/Input/cake_hard.png ${ITK_TEST_OUTPUT_DIR}/itkDifferenceImageFilterTest.png 10 1)
