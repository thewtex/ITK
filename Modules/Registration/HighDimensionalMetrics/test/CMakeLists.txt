itk_module_test()
set(ITKHighDimensionalMetricsTests
  itkEuclideanDistancePointSetMetricTest.cxx
  itkExpectationBasedPointSetMetricTest.cxx
  itkJensenHavrdaCharvatTsallisPointSetMetricTest.cxx
  itkObjectToObjectMetricTest.cxx
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
              ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
              ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBSplined10.png
              ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest.nii.gz
              10 5 )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2.nii.gz
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest2.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3.nii.gz
      --compareIntensityTolerance 2.5
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest3.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4.nii.gz
      --compareIntensityTolerance 0.25
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest4.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest5
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest5.nii.gz
      --compareIntensityTolerance 0.025
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest5.nii.gz
      50 50
      1 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest6
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest6.nii.gz
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest6.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest7
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest7.nii.gz
      --compareIntensityTolerance 2.5
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest7.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest8
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest8.nii.gz
      --compareIntensityTolerance 0.25
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest8.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest9
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_face_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest9.nii.gz
      --compareIntensityTolerance 0.025
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/face_avg.jpg}
      DATA{Input/face_b.jpg}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest9.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest10
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_slice_50_50_2_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest10.nii.gz
      --compareIntensityTolerance 0.25
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/r16slice.nii.gz}
      DATA{Input/r64slice.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest10.nii.gz
      50 50
      2 #sparseSamplingStep
      2 #shrinkFactor
      1 #numberOfThreads
      )

itk_add_test(NAME itkJointHistogramMutualInformationImageToImageObjectRegistrationTest11
      COMMAND ITKHighDimensionalMetricsTestDriver
      --compare
      DATA{Baseline/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest_slice_50_50_1_2_1.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest11.nii.gz
      --compareIntensityTolerance 0.25
      itkJointHistogramMutualInformationImageToImageObjectRegistrationTest
      DATA{Input/r16slice.nii.gz}
      DATA{Input/r64slice.nii.gz}
      ${TEMP}/itkJointHistogramMutualInformationImageToImageObjectRegistrationTest11.nii.gz
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
              ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
              ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBSplined10.png
             ${TEMP}/itkANTSNeighborhoodCorrelationImageToImageObjectRegistrationTest.nii.gz
              10 1 0.25 )
