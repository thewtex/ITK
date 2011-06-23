itk_module_test()
set(ITK_SpatialFunctionTests
itkSpatialFunctionImageEvaluatorFilterTest.cxx
itkSpatialFunctionHeaderTest.cxx
)

CreateTestDriver(ITK_SpatialFunction  "${ITK_SpatialFunction-Test_LIBRARIES}" "${ITK_SpatialFunctionTests}")

itk_add_test(NAME itkSpatialFunctionHeaderTest
      COMMAND ITK_SpatialFunctionTestDriver itkSpatialFunctionHeaderTest)
itk_add_test(NAME itkSpatialFunctionImageEvaluatorFilterTest
      COMMAND ITK_SpatialFunctionTestDriver itkSpatialFunctionImageEvaluatorFilterTest)
