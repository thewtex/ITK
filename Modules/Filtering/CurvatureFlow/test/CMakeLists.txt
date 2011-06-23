itk_module_test()
set(ITK_CurvatureFlowTests
itkBinaryMinMaxCurvatureFlowImageFilterTest.cxx
itkCurvatureFlowHeaderTest.cxx
itkCurvatureFlowTest.cxx
)

CreateTestDriver(ITK_CurvatureFlow  "${ITK_CurvatureFlow-Test_LIBRARIES}" "${ITK_CurvatureFlowTests}")

itk_add_test(NAME itkCurvatureFlowHeaderTest
      COMMAND ITK_CurvatureFlowTestDriver itkCurvatureFlowHeaderTest)
itk_add_test(NAME itkBinaryMinMaxCurvatureFlowImageFilterTest
      COMMAND ITK_CurvatureFlowTestDriver itkBinaryMinMaxCurvatureFlowImageFilterTest)
itk_add_test(NAME itkCurvatureFlowTesti
      COMMAND ITK_CurvatureFlowTestDriver itkCurvatureFlowTest ${ITK_TEST_OUTPUT_DIR}/itkCurvatureFlowTest.vtk)
