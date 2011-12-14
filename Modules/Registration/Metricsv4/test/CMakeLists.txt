itk_module_test()
set(ITKMetricsv4Tests
  itkEuclideanDistancePointSetMetricTest.cxx
  itkExpectationBasedPointSetMetricTest.cxx
  itkJensenHavrdaCharvatTsallisPointSetMetricTest.cxx
  itkImageToImageMetricv4Test.cxx
  itkJointHistogramMutualInformationImageToImageMetricv4Test.cxx
  itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.cxx
  itkDemonsImageToImageMetricv4Test.cxx
  itkANTSNeighborhoodCorrelationImageToImageMetricv4Test.cxx
  itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest.cxx
  itkAffineDemonsImageToImageObjectRegistrationTest.cxx
)

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITKMetricsv4  "${ITKMetricsv4-Test_LIBRARIES}" "${ITKMetricsv4Tests}")

itk_add_test(NAME itkEuclideanDistancePointSetMetricTest
      COMMAND ITKMetricsv4TestDriver itkEuclideanDistancePointSetMetricTest)

itk_add_test(NAME itkExpectationBasedPointSetMetricTest
      COMMAND ITKMetricsv4TestDriver itkExpectationBasedPointSetMetricTest)

itk_add_test(NAME itkJensenHavrdaCharvatTsallisPointSetMetricTest
      COMMAND ITKMetricsv4TestDriver itkJensenHavrdaCharvatTsallisPointSetMetricTest)

itk_add_test(NAME itkImageToImageMetricv4Test
      COMMAND ITKMetricsv4TestDriver
              itkImageToImageMetricv4Test)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageMetricv4Test
      COMMAND ITKMetricsv4TestDriver
              itkJointHistogramMutualInformationImageToImageMetricv4Test)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      COMMAND ITKMetricsv4TestDriver
              itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBorder20.png
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBSplined10.png
              ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.nii.gz
              10 5 )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2
      COMMAND ITKMetricsv4TestDriver
              itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
              DATA{Baseline/face_avg.jpg}
              DATA{Input/face_b.jpg}
              ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2.nii.gz
              50 0 )

itk_add_test(NAME itkDemonsImageToImageMetricv4Test
      COMMAND ITKMetricsv4TestDriver
      itkDemonsImageToImageMetricv4Test)

itk_add_test(NAME itkANTSNeighborhoodCorrelationImageToImageMetricv4Test
      COMMAND ITKMetricsv4TestDriver
              itkANTSNeighborhoodCorrelationImageToImageMetricv4Test)

itk_add_test(NAME itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest
      COMMAND ITKMetricsv4TestDriver
              itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBorder20.png
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBSplined10.png
             ${TEMP}/itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest.nii.gz
              10 1 0.25 )

itk_add_test(NAME itkAffineDemonsImageToImageObjectRegistrationTest
      COMMAND ITKMetricsv4TestDriver
              itkAffineDemonsImageToImageObjectRegistrationTest
              DATA{Baseline/face_avg.jpg}
              DATA{Input/face_b.jpg}
              ${TEMP}/itkAffineDemonsImageToImageObjectRegistrationTest.nii.gz
              30 )
