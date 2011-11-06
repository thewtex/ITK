itk_module_test()
set(ITKHighDimensionalMetricsTests
  itkEuclideanDistancePointSetMetricTest.cxx
  itkExpectationBasedPointSetMetricTest.cxx
  itkJensenHavrdaCharvatTsallisPointSetMetricTest.cxx
  itkImageToImageObjectMetricTest.cxx
  itkJointHistogramMutualInformationImageToImageObjectMetricTest.cxx
  itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.cxx
  itkDemonsImageToImageObjectMetricTest.cxx
  itkANTSNeighborhoodCorrelationImageToImageObjectMetricTest.cxx
  itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest.cxx
)

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITKHighDimensionalMetrics  "${ITKHighDimensionalMetrics-Test_LIBRARIES}" "${ITKHighDimensionalMetricsTests}")

itk_add_test(NAME itkEuclideanDistancePointSetMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver itkEuclideanDistancePointSetMetricTest)

itk_add_test(NAME itkExpectationBasedPointSetMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver itkExpectationBasedPointSetMetricTest)

itk_add_test(NAME itkJensenHavrdaCharvatTsallisPointSetMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver itkJensenHavrdaCharvatTsallisPointSetMetricTest)

itk_add_test(NAME itkImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkImageToImageObjectMetricTest)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkJointHistogramMutualInformationImageToImageObjectMetricTest)

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.nii.gz
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2.nii.gz
      --compareIntensityTolerance 2.5
      --compareNumberOfPixelsTolerance 400
      --compareRadiusTolerance 2
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_slice_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3.nii.gz
      --compareIntensityTolerance 2.5
      --compareNumberOfPixelsTolerance 600
      --compareRadiusTolerance 2
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/r16slice.nii.gz}
      DATA{Input/r64slice.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_slice_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4.nii.gz
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/r16slice.nii.gz}
      DATA{Input/r64slice.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkDemonsImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
      itkDemonsImageToImageObjectMetricTest)

itk_add_test(NAME itkANTSNeighborhoodCorrelationImageToImageObjectMetricTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkANTSNeighborhoodCorrelationImageToImageObjectMetricTest)

itk_add_test(NAME itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest
      COMMAND ITKHighDimensionalMetricsTestDriver
              itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBorder20.png
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBSplined10.png
             ${TEMP}/itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest.nii.gz
              10 1 0.25 )
