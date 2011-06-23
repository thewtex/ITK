itk_module_test()
set(ITK_StatisticsTests
itkDecisionRuleTest.cxx
itkDenseFrequencyContainer2Test.cxx
itkExpectationMaximizationMixtureModelEstimatorTest.cxx
itkGaussianDistributionTest.cxx
itkGaussianMembershipFunctionTest.cxx
itkGaussianMixtureModelComponentTest.cxx
itkKalmanLinearEstimatorTest.cxx
itkKdTreeBasedKmeansEstimatorTest.cxx
itkKdTreeGeneratorTest.cxx
itkKdTreeTest1.cxx
itkKdTreeTest2.cxx
itkKdTreeTest3.cxx
itkKdTreeTestSamplePoints.cxx
itkStatisticsHeaderTest.cxx
itkMaximumDecisionRuleTest.cxx
itkMinimumDecisionRuleTest.cxx
itkMaximumRatioDecisionRuleTest.cxx
itkMeanSampleFilterTest.cxx
itkMeanSampleFilterTest2.cxx
itkMeanSampleFilterTest3.cxx
itkHistogramTest.cxx
itkHistogramToTextureFeaturesFilterTest.cxx
itkChiSquareDistributionTest.cxx
itkCovarianceSampleFilterTest.cxx
itkCovarianceSampleFilterTest2.cxx
itkCovarianceSampleFilterTest3.cxx
itkWeightedCovarianceSampleFilterTest.cxx
itkWeightedCovarianceSampleFilterTest2.cxx
itkWeightedMeanSampleFilterTest.cxx
itkWeightedCentroidKdTreeGeneratorTest1.cxx
itkWeightedCentroidKdTreeGeneratorTest8.cxx
itkWeightedCentroidKdTreeGeneratorTest9.cxx
itkMahalanobisDistanceMetricTest.cxx
itkManhattanDistanceMetricTest.cxx
itkMembershipFunctionBaseTest.cxx
itkMembershipFunctionBaseTest2.cxx
itkMembershipSampleTest1.cxx
itkMembershipSampleTest2.cxx
itkMembershipSampleTest3.cxx
itkMembershipSampleTest4.cxx
itkMeasurementVectorTraitsTest.cxx
itkNeighborhoodSamplerTest1.cxx
itkMixtureModelComponentBaseTest.cxx
itkNormalVariateGeneratorTest1.cxx
itkDistanceMetricTest.cxx
itkDistanceMetricTest2.cxx
itkDistanceToCentroidMembershipFunctionTest.cxx
itkEuclideanDistanceMetricTest.cxx
itkEuclideanSquareDistanceMetricTest.cxx
itkListSampleTest.cxx
itkImageToListSampleAdaptorTest.cxx
itkImageToListSampleAdaptorTest2.cxx
itkImageToListSampleFilterTest.cxx
itkImageToListSampleFilterTest2.cxx
itkImageToListSampleFilterTest3.cxx
itkListSampleTest.cxx
itkJointDomainImageToListSampleAdaptorTest.cxx
itkPointSetToListSampleAdaptorTest.cxx
itkProbabilityDistributionTest.cxx
itkRandomVariateGeneratorBaseTest.cxx
itkSampleTest.cxx
itkSampleTest2.cxx
itkSampleTest3.cxx
itkSampleTest4.cxx
itkSampleToHistogramFilterTest.cxx
itkSampleToHistogramFilterTest2.cxx
itkSampleToHistogramFilterTest3.cxx
itkSampleToHistogramFilterTest4.cxx
itkSampleToHistogramFilterTest5.cxx
itkSampleToHistogramFilterTest6.cxx
itkSampleToHistogramFilterTest7.cxx
itkSampleToSubsampleFilterTest1.cxx
itkScalarImageToCooccurrenceListSampleFilterTest.cxx
itkScalarImageToCooccurrenceMatrixFilterTest.cxx
itkScalarImageToCooccurrenceMatrixFilterTest2.cxx
itkScalarImageToTextureFeaturesFilterTest.cxx
itkSparseFrequencyContainer2Test.cxx
itkStandardDeviationPerComponentSampleFilterTest.cxx
itkStatisticsTypesTest.cxx
itkSubsampleTest.cxx
itkSubsampleTest2.cxx
itkSubsampleTest3.cxx
itkTDistributionTest.cxx
itkStatisticsAlgorithmTest.cxx
itkStatisticsAlgorithmTest2.cxx
)

CreateTestDriver(ITK_Statistics  "${ITK_Statistics-Test_LIBRARIES}" "${ITK_StatisticsTests}")

itk_add_test(NAME itkStatisticsHeaderTest
      COMMAND ITK_StatisticsTestDriver itkStatisticsHeaderTest)
itk_add_test(NAME itkDecisionRuleTest
      COMMAND ITK_StatisticsTestDriver itkDecisionRuleTest)
itk_add_test(NAME itkDenseFrequencyContainer2Test
      COMMAND ITK_StatisticsTestDriver itkDenseFrequencyContainer2Test)
itk_add_test(NAME itkExpectationMaximizationMixtureModelEstimatorTest
      COMMAND ITK_StatisticsTestDriver itkExpectationMaximizationMixtureModelEstimatorTest
              ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)
itk_add_test(NAME itkGaussianDistributionTest
      COMMAND ITK_StatisticsTestDriver itkGaussianDistributionTest)
itk_add_test(NAME itkGaussianMembershipFunctionTest
      COMMAND ITK_StatisticsTestDriver itkGaussianMembershipFunctionTest)
itk_add_test(NAME itkGaussianMixtureModelComponentTest
      COMMAND ITK_StatisticsTestDriver itkGaussianMixtureModelComponentTest
              ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)
itk_add_test(NAME itkKalmanLinearEstimatorTest
      COMMAND ITK_StatisticsTestDriver itkKalmanLinearEstimatorTest)
itk_add_test(NAME itkKdTreeBasedKmeansEstimatorTest
      COMMAND ITK_StatisticsTestDriver itkKdTreeBasedKmeansEstimatorTest
              ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat 1 28.54746 0.07)
itk_add_test(NAME itkKdTreeGeneratorTest
      COMMAND ITK_StatisticsTestDriver itkKdTreeGeneratorTest
              ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)
itk_add_test(NAME itkKdTreeTest1
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              100 100 1 ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest1_100.dot)
itk_add_test(NAME itkKdTreeTest2
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest2
              ${ITK_DATA_ROOT}/Input/pointsForKdTree.txt 1 ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest2.dot)
itk_add_test(NAME itkKdTreeTest3
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest2
              ${ITK_DATA_ROOT}/Input/pointsForKdTree2.txt 1 ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest3.dot)
itk_add_test(NAME itkKdTreeTest4
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 1)
itk_add_test(NAME itkKdTreeTest5
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 2)
itk_add_test(NAME itkKdTreeTest6
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 3)
itk_add_test(NAME itkKdTreeTest7
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 5)
itk_add_test(NAME itkKdTreeTest8
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 10)
itk_add_test(NAME itkKdTreeTest9
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 20)
itk_add_test(NAME itkKdTreeTest10
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest1
              1000 1000 100)
itk_add_test(NAME itkKdTreeTest11
      COMMAND ITK_StatisticsTestDriver itkKdTreeTest3
              1000 100 20 16)

#
#  The following tests generate graphical plots of the KdTree organization.
#  They are very helpful for debugging and verification purposes, but not so
#  much for regression testing. They need the "dot" tool that is part of the
#  Graphviz package. The dot executable is found as part of the process of
#  configuring the Doxygen documentation. Therefore, to enable these tests
#  below, you must enable BUILD_DOXYGEN and BUILD_DOCUMENTATION.
#
if(DOXYGEN_DOT_EXECUTABLE)
itk_add_test(NAME itkKdTreeTest1Plot
      COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest1_100.png ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest1_100.dot)
set_tests_properties( itkKdTreeTest1Plot PROPERTIES DEPENDS itkKdTreeTest1 )
itk_add_test(NAME itkKdTreeTest2Plot
      COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest2.png ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest2.dot)
set_tests_properties( itkKdTreeTest2Plot PROPERTIES DEPENDS itkKdTreeTest2 )
itk_add_test(NAME itkKdTreeTest3Plot
      COMMAND ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest3.png ${ITK_TEST_OUTPUT_DIR}/itkKdTreeTest3.dot)
set_tests_properties( itkKdTreeTest3Plot PROPERTIES DEPENDS itkKdTreeTest3 )
endif(DOXYGEN_DOT_EXECUTABLE)

itk_add_test(NAME itkKdTreeTestSamplePoints
      COMMAND ITK_StatisticsTestDriver itkKdTreeTestSamplePoints)
itk_add_test(NAME itkMaximumDecisionRuleTest
      COMMAND ITK_StatisticsTestDriver itkMaximumDecisionRuleTest)
itk_add_test(NAME itkMinimumDecisionRuleTest
      COMMAND ITK_StatisticsTestDriver itkMinimumDecisionRuleTest)
itk_add_test(NAME itkMaximumRatioDecisionRuleTest
      COMMAND ITK_StatisticsTestDriver itkMaximumRatioDecisionRuleTest)
itk_add_test(NAME itkMeanSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkMeanSampleFilterTest)
itk_add_test(NAME itkMeanSampleFilterTest2
      COMMAND ITK_StatisticsTestDriver itkMeanSampleFilterTest2)
itk_add_test(NAME itkMeanSampleFilterTest3
      COMMAND ITK_StatisticsTestDriver itkMeanSampleFilterTest3)
itk_add_test(NAME itkHistogramTest
      COMMAND ITK_StatisticsTestDriver itkHistogramTest)
itk_add_test(NAME itkHistogramToTextureFeaturesFilterTest
      COMMAND ITK_StatisticsTestDriver itkHistogramToTextureFeaturesFilterTest)
itk_add_test(NAME itkChiSquareDistributionTest
      COMMAND ITK_StatisticsTestDriver itkChiSquareDistributionTest)
itk_add_test(NAME itkCovarianceSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkCovarianceSampleFilterTest)
itk_add_test(NAME itkCovarianceSampleFilterTest2
      COMMAND ITK_StatisticsTestDriver itkCovarianceSampleFilterTest2)
itk_add_test(NAME itkCovarianceSampleFilterTest3
      COMMAND ITK_StatisticsTestDriver itkCovarianceSampleFilterTest3)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest1
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 1)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest2
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 2)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest3
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 3)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest4
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 5)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest5
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 10)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest6
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 20)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest7
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest1
              1000 1000 100)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest8
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest8
              1000 1000 1)
itk_add_test(NAME itkWeightedCentroidKdTreeGeneratorTest9
      COMMAND ITK_StatisticsTestDriver itkWeightedCentroidKdTreeGeneratorTest9
              1000 1000 1)
itk_add_test(NAME itkWeightedCovarianceSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkWeightedCovarianceSampleFilterTest)
itk_add_test(NAME itkWeightedCovarianceSampleFilterTest2
      COMMAND ITK_StatisticsTestDriver itkWeightedCovarianceSampleFilterTest2)
itk_add_test(NAME itkWeightedMeanSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkWeightedMeanSampleFilterTest)
itk_add_test(NAME itkDistanceMetricTest
      COMMAND ITK_StatisticsTestDriver itkDistanceMetricTest)
itk_add_test(NAME itkDistanceMetricTest2
      COMMAND ITK_StatisticsTestDriver itkDistanceMetricTest2)
itk_add_test(NAME itkDistanceToCentroidMembershipFunctionTest
      COMMAND ITK_StatisticsTestDriver itkDistanceToCentroidMembershipFunctionTest)
itk_add_test(NAME itkEuclideanDistanceMetricTest
      COMMAND ITK_StatisticsTestDriver itkEuclideanDistanceMetricTest)
itk_add_test(NAME itkEuclideanSquareDistanceMetricTest
      COMMAND ITK_StatisticsTestDriver itkEuclideanSquareDistanceMetricTest)
itk_add_test(NAME itkListSampleTest1
      COMMAND ITK_StatisticsTestDriver itkListSampleTest
              1)
itk_add_test(NAME itkListSampleTest2
      COMMAND ITK_StatisticsTestDriver itkListSampleTest
              5)
itk_add_test(NAME itkListSampleTest3
      COMMAND ITK_StatisticsTestDriver itkListSampleTest
              10)
itk_add_test(NAME itkListSampleTest4
      COMMAND ITK_StatisticsTestDriver itkListSampleTest
              100)
itk_add_test(NAME itkImageToListSampleAdaptorTest
      COMMAND ITK_StatisticsTestDriver itkImageToListSampleAdaptorTest)
itk_add_test(NAME itkImageToListSampleAdaptorTest2
      COMMAND ITK_StatisticsTestDriver itkImageToListSampleAdaptorTest2)
itk_add_test(NAME itkImageToListSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkImageToListSampleFilterTest)
itk_add_test(NAME itkImageToListSampleFilterTest2
      COMMAND ITK_StatisticsTestDriver itkImageToListSampleFilterTest2)
itk_add_test(NAME itkImageToListSampleFilterTest3
      COMMAND ITK_StatisticsTestDriver itkImageToListSampleFilterTest3)
itk_add_test(NAME itkJointDomainImageToListSampleAdaptorTest
      COMMAND ITK_StatisticsTestDriver itkJointDomainImageToListSampleAdaptorTest)
itk_add_test(NAME itkPointSetToListSampleAdaptorTest
      COMMAND ITK_StatisticsTestDriver itkPointSetToListSampleAdaptorTest)
itk_add_test(NAME itkProbabilityDistributionTest
      COMMAND ITK_StatisticsTestDriver itkProbabilityDistributionTest)
itk_add_test(NAME itkRandomVariateGeneratorBaseTest
      COMMAND ITK_StatisticsTestDriver itkRandomVariateGeneratorBaseTest)
itk_add_test(NAME itkMahalanobisDistanceMetricTest
      COMMAND ITK_StatisticsTestDriver itkMahalanobisDistanceMetricTest)
itk_add_test(NAME itkManhattanDistanceMetricTest
      COMMAND ITK_StatisticsTestDriver itkManhattanDistanceMetricTest)
itk_add_test(NAME itkMembershipFunctionBaseTest
      COMMAND ITK_StatisticsTestDriver itkMembershipFunctionBaseTest)
itk_add_test(NAME itkMembershipFunctionBaseTest2
      COMMAND ITK_StatisticsTestDriver itkMembershipFunctionBaseTest2)
itk_add_test(NAME itkMembershipSampleTest1
      COMMAND ITK_StatisticsTestDriver itkMembershipSampleTest1)
itk_add_test(NAME itkMembershipSampleTest2
      COMMAND ITK_StatisticsTestDriver itkMembershipSampleTest2)
itk_add_test(NAME itkMembershipSampleTest3
      COMMAND ITK_StatisticsTestDriver itkMembershipSampleTest3)
itk_add_test(NAME itkMembershipSampleTest4
      COMMAND ITK_StatisticsTestDriver itkMembershipSampleTest4)
itk_add_test(NAME itkMeasurementVectorTraitsTest
      COMMAND ITK_StatisticsTestDriver itkMeasurementVectorTraitsTest)
itk_add_test(NAME itkNeighborhoodSamplerTest1
      COMMAND ITK_StatisticsTestDriver itkNeighborhoodSamplerTest1)
itk_add_test(NAME itkMixtureModelComponentBaseTest
      COMMAND ITK_StatisticsTestDriver itkMixtureModelComponentBaseTest)
itk_add_test(NAME itkNormalVariateGeneratorTest1
      COMMAND ITK_StatisticsTestDriver itkNormalVariateGeneratorTest1)
itk_add_test(NAME itkSampleTest
      COMMAND ITK_StatisticsTestDriver itkSampleTest)
itk_add_test(NAME itkSampleTest2
      COMMAND ITK_StatisticsTestDriver itkSampleTest2)
itk_add_test(NAME itkSampleTest3
      COMMAND ITK_StatisticsTestDriver itkSampleTest3)
itk_add_test(NAME itkSampleTest4
      COMMAND ITK_StatisticsTestDriver itkSampleTest4)
itk_add_test(NAME itkSampleToHistogramFilterTest
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest)
itk_add_test(NAME itkSampleToHistogramFilterTest2
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest2)
itk_add_test(NAME itkSampleToHistogramFilterTest3
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest3)
itk_add_test(NAME itkSampleToHistogramFilterTest4
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest4)
itk_add_test(NAME itkSampleToHistogramFilterTest5
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest5
              ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png)
itk_add_test(NAME itkSampleToHistogramFilterTest6
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest6)
itk_add_test(NAME itkSampleToHistogramFilterTest7
      COMMAND ITK_StatisticsTestDriver itkSampleToHistogramFilterTest7)
itk_add_test(NAME itkSampleToSubsampleFilterTest1
      COMMAND ITK_StatisticsTestDriver itkSampleToSubsampleFilterTest1)
itk_add_test(NAME itkScalarImageToCooccurrenceListSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkScalarImageToCooccurrenceListSampleFilterTest)
itk_add_test(NAME itkScalarImageToCooccurrenceMatrixFilterTest
      COMMAND ITK_StatisticsTestDriver itkScalarImageToCooccurrenceMatrixFilterTest)
itk_add_test(NAME itkScalarImageToCooccurrenceMatrixFilterTest2
      COMMAND ITK_StatisticsTestDriver itkScalarImageToCooccurrenceMatrixFilterTest2)
itk_add_test(NAME itkScalarImageToTextureFeaturesFilterTest
      COMMAND ITK_StatisticsTestDriver itkScalarImageToTextureFeaturesFilterTest)
itk_add_test(NAME itkSparseFrequencyContainer2Test
      COMMAND ITK_StatisticsTestDriver itkSparseFrequencyContainer2Test)
itk_add_test(NAME itkStandardDeviationPerComponentSampleFilterTest
      COMMAND ITK_StatisticsTestDriver itkStandardDeviationPerComponentSampleFilterTest)
itk_add_test(NAME itkStatisticsTypesTest
      COMMAND ITK_StatisticsTestDriver itkStatisticsTypesTest)
itk_add_test(NAME itkSubsampleTest
      COMMAND ITK_StatisticsTestDriver itkSubsampleTest)
itk_add_test(NAME itkSubsampleTest2
      COMMAND ITK_StatisticsTestDriver itkSubsampleTest2)
itk_add_test(NAME itkSubsampleTest3
      COMMAND ITK_StatisticsTestDriver itkSubsampleTest3)
itk_add_test(NAME itkTDistributionTest
      COMMAND ITK_StatisticsTestDriver itkTDistributionTest)
itk_add_test(NAME itkStatisticsAlgorithmTest
      COMMAND ITK_StatisticsTestDriver itkStatisticsAlgorithmTest)
itk_add_test(NAME itkStatisticsAlgorithmTest2
      COMMAND ITK_StatisticsTestDriver itkStatisticsAlgorithmTest2)
