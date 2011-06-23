itk_module_test()
set(ITK_PathTests
itkPathIteratorTest.cxx
itkPathHeaderTest.cxx
itkChainCodePathTest.cxx
itkChainCodePath2DTest.cxx
itkOrthogonallyCorrected2DParametricPathTest.cxx
itkPathToImageFilterTest.cxx
itkPathToChainCodePathFilterTest.cxx
itkFourierSeriesPathTest.cxx
itkPathFunctionsTest.cxx
itkPolyLineParametricPathTest.cxx
itkChainCodeToFourierSeriesPathFilterTest.cxx
itkExtractOrthogonalSwath2DImageFilterTest.cxx
itkOrthogonalSwath2DPathFilterTest.cxx
)

CreateTestDriver(ITK_Path  "${ITK_Path-Test_LIBRARIES}" "${ITK_PathTests}")

itk_add_test(NAME itkPathHeaderTest
      COMMAND ITK_PathTestDriver itkPathHeaderTest)
itk_add_test(NAME itkPathIteratorTest
      COMMAND ITK_PathTestDriver itkPathIteratorTest)
itk_add_test(NAME itkChainCodePathTest
      COMMAND ITK_PathTestDriver itkChainCodePathTest)
itk_add_test(NAME itkChainCodePath2DTest
      COMMAND ITK_PathTestDriver itkChainCodePath2DTest)
itk_add_test(NAME itkOrthogonallyCorrected2DParametricPathTest
      COMMAND ITK_PathTestDriver itkOrthogonallyCorrected2DParametricPathTest)
itk_add_test(NAME itkPathToImageFilterTest
      COMMAND ITK_PathTestDriver itkPathToImageFilterTest)
itk_add_test(NAME itkPathToChainCodePathFilterTest
      COMMAND ITK_PathTestDriver itkPathToChainCodePathFilterTest)
itk_add_test(NAME itkFourierSeriesPathTest
      COMMAND ITK_PathTestDriver itkFourierSeriesPathTest)
itk_add_test(NAME itkPathFunctionsTest
      COMMAND ITK_PathTestDriver itkPathFunctionsTest)
itk_add_test(NAME itkPolyLineParametricPathTest
      COMMAND ITK_PathTestDriver itkPolyLineParametricPathTest)
itk_add_test(NAME itkChainCodeToFourierSeriesPathFilterTest
      COMMAND ITK_PathTestDriver itkChainCodeToFourierSeriesPathFilterTest)
itk_add_test(NAME itkExtractOrthogonalSwath2DImageFilterTest
      COMMAND ITK_PathTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ExtractOrthogonalSwath2DImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/ExtractOrthogonalSwath2DImageFilterTest.png
    itkExtractOrthogonalSwath2DImageFilterTest ${ITK_TEST_OUTPUT_DIR}/ExtractOrthogonalSwath2DImageFilterTest.png)
itk_add_test(NAME itkOrthogonalSwath2DPathFilterTest
      COMMAND ITK_PathTestDriver itkOrthogonalSwath2DPathFilterTest)
