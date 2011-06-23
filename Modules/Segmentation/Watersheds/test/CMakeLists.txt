itk_module_test()
set(ITK_WatershedsTests
itkTobogganImageFilterTest.cxx
itkWatershedsHeaderTest.cxx
itkIsolatedWatershedImageFilterTest.cxx
itkWatershedImageFilterTest.cxx
)

CreateTestDriver(ITK_Watersheds  "${ITK_Watersheds-Test_LIBRARIES}" "${ITK_WatershedsTests}")

itk_add_test(NAME itkWatershedsHeaderTest
      COMMAND ITK_WatershedsTestDriver itkWatershedsHeaderTest)
itk_add_test(NAME itkTobogganImageFilterTest
      COMMAND ITK_WatershedsTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/TobogganImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/TobogganImageFilterTest.png
    itkTobogganImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/TobogganImageFilterTest.png)
itk_add_test(NAME itkIsolatedWatershedImageFilterTest
      COMMAND ITK_WatershedsTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/IsolatedWatershedImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/IsolatedWatershedImageFilterTest.png
    itkIsolatedWatershedImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/IsolatedWatershedImageFilterTest.png 113 84 120 99)
itk_add_test(NAME itkWatershedImageFilterTest
      COMMAND ITK_WatershedsTestDriver itkWatershedImageFilterTest)
