itk_module_test()
set(ITKHighDimensionalMetricsTests
  itkObjectToObjectMetricTest.cxx
  itkImageToImageObjectMetricTest.cxx
  itkJointHistogramMutualInformationImageToImageObjectMetricTest.cxx
  itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.cxx
)

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITKHighDimensionalMetrics  "${ITKHighDimensionalMetrics-Test_LIBRARIES}" "${ITKHighDimensionalMetricsTests}")

itk_add_test(NAME itkObjectToObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver itkObjectToObjectMetricTest)

itk_add_test(NAME itkImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkImageToImageObjectMetricTest)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkJointHistogramMutualInformationImageToImageObjectMetricTest)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
              ${ITK_DATA_ROOT}/BrainProtonDensitySliceBorder20.png
              ${ITK_DATA_ROOT}/BrainProtonDensitySliceBSplined10.png
              ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.nii.gz
              10 1 0.25 )
