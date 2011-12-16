itk_module_test()
set(ITKMetricsv4Tests
  itkPreWarpTest.cxx
)

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITKMetricsv4  "${ITKMetricsv4-Test_LIBRARIES}" "${ITKMetricsv4Tests}")

itk_add_test(NAME itkPreWarpTest
      COMMAND ITKMetricsv4TestDriver itkPreWarpTest )
