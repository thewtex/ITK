Find_PACKAGE(TCL)
mark_as_advanced(TCL_TCLSH)

if(TCL_TCLSH)
  add_test(PrintSelf-Statistics ${TCL_TCLSH}
           ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl
           ${ITK_SOURCE_DIR}/Code/Numerics/Statistics)
endif(TCL_TCLSH)

if( NOT ITK_DISABLE_CXX_TESTING )
set(TEMP ${ITK_BINARY_DIR}/Testing/Temporary)

set(KIT Statistics)
set(CXX_TESTS ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)

set(KIT_LIBS ITKIO ITKCommon ITKNumerics  ITKStatistics)
set(KitTests
  itkChiSquareDistributionTest.cxx
  itkCovarianceSampleFilterTest.cxx
  itkCovarianceSampleFilterTest2.cxx
  itkCovarianceSampleFilterTest3.cxx
  itkDecisionRuleTest.cxx
  itkDenseFrequencyContainer2Test.cxx
  itkDistanceMetricTest.cxx
  itkDistanceMetricTest2.cxx
  itkDistanceToCentroidMembershipFunctionTest.cxx
  itkEuclideanDistanceMetricTest.cxx
  itkEuclideanSquareDistanceMetricTest.cxx
  itkExpectationMaximizationMixtureModelEstimatorTest.cxx
  itkGaussianDistributionTest.cxx
  itkGaussianMembershipFunctionTest.cxx
  itkGaussianMixtureModelComponentTest.cxx
  itkHistogramTest.cxx
  itkHistogramToEntropyImageFilterTest1.cxx
  itkHistogramToEntropyImageFilterTest2.cxx
  itkHistogramToIntensityImageFilterTest1.cxx
  itkHistogramToIntensityImageFilterTest2.cxx
  itkHistogramToLogProbabilityImageFilterTest1.cxx
  itkHistogramToLogProbabilityImageFilterTest2.cxx
  itkHistogramToProbabilityImageFilterTest1.cxx
  itkHistogramToProbabilityImageFilterTest2.cxx
  itkHistogramToTextureFeaturesFilterTest.cxx
  itkImageClassifierFilterTest.cxx
  itkImageToHistogramFilterTest.cxx
  itkImageToHistogramFilterTest2.cxx
  itkImageToHistogramFilterTest3.cxx
  itkImageToListSampleAdaptorTest.cxx
  itkImageToListSampleAdaptorTest2.cxx
  itkImageToListSampleFilterTest.cxx
  itkImageToListSampleFilterTest2.cxx
  itkImageToListSampleFilterTest3.cxx
  itkJointDomainImageToListSampleAdaptorTest.cxx
  itkKdTreeBasedKmeansEstimatorTest.cxx
  itkKdTreeGeneratorTest.cxx
  itkKdTreeTest1.cxx
  itkKdTreeTest2.cxx
  itkKdTreeTestSamplePoints.cxx
  itkListSampleTest.cxx
  itkMahalanobisDistanceMetricTest.cxx
  itkManhattanDistanceMetricTest.cxx
  itkMaximumDecisionRuleTest.cxx
  itkMaximumRatioDecisionRuleTest.cxx
  itkMeanSampleFilterTest.cxx
  itkMeanSampleFilterTest2.cxx
  itkMeanSampleFilterTest3.cxx
  itkMeasurementVectorTraitsTest.cxx
  itkMembershipFunctionBaseTest.cxx
  itkMembershipFunctionBaseTest2.cxx
  itkMembershipSampleTest1.cxx
  itkMembershipSampleTest2.cxx
  itkMembershipSampleTest3.cxx
  itkMembershipSampleTest4.cxx
  itkMinimumDecisionRuleTest.cxx
  itkMixtureModelComponentBaseTest.cxx
  itkNeighborhoodSamplerTest1.cxx
  itkNormalVariateGeneratorTest1.cxx
  itkPointSetToListSampleAdaptorTest.cxx
  itkProbabilityDistributionTest.cxx
  itkRandomVariateGeneratorBaseTest.cxx
  itkSampleClassifierFilterTest1.cxx
  itkSampleClassifierFilterTest2.cxx
  itkSampleClassifierFilterTest3.cxx
  itkSampleClassifierFilterTest4.cxx
  itkSampleClassifierFilterTest5.cxx
  itkSampleClassifierFilterTest6.cxx
  itkSampleClassifierFilterTest7.cxx
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
  itkStatisticsAlgorithmTest.cxx
  itkStatisticsAlgorithmTest2.cxx
  itkStatisticsPrintTest.cxx
  itkStatisticsTypesTest.cxx
  itkSubsampleTest.cxx
  itkSubsampleTest2.cxx
  itkSubsampleTest3.cxx
  itkTDistributionTest.cxx
  itkWeightedCentroidKdTreeGeneratorTest1.cxx
  itkWeightedCentroidKdTreeGeneratorTest8.cxx
  itkWeightedCentroidKdTreeGeneratorTest9.cxx
  itkWeightedCovarianceSampleFilterTest.cxx
  itkWeightedCovarianceSampleFilterTest2.cxx
  itkWeightedMeanSampleFilterTest.cxx
  )

# Given KIT, and KitTests, defines Tests
include(${ITK_SOURCE_DIR}/Testing/Utilities/TestDriver.cmake)

add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT_LIBS})

set(STATISTICS_HEADER_TESTS ${EXECUTABLE_OUTPUT_PATH}/itkStatisticsHeaderTest)
add_executable(itkStatisticsHeaderTest itkStatisticsHeaderTest.cxx)
target_link_libraries(itkStatisticsHeaderTest ${KIT_LIBS})

add_test(itkStatisticsHeaderTest ${STATISTICS_HEADER_TESTS})
add_test(itkStatisticsPrintTest ${CXX_TESTS} itkStatisticsPrintTest)
add_test(itkStatisticsTypesTest ${CXX_TESTS} itkStatisticsTypesTest)

add_test(itkCovarianceSampleFilterTest ${CXX_TESTS}
         itkCovarianceSampleFilterTest)
add_test(itkCovarianceSampleFilterTest2 ${CXX_TESTS}
         itkCovarianceSampleFilterTest2)
add_test(itkCovarianceSampleFilterTest3 ${CXX_TESTS}
         itkCovarianceSampleFilterTest3)
add_test(itkHistogramToTextureFeaturesFilterTest ${CXX_TESTS}
         itkHistogramToTextureFeaturesFilterTest)
add_test(itkHistogramTest ${CXX_TESTS}
         itkHistogramTest)
add_test(itkHistogramToEntropyImageFilterTest1 ${CXX_TESTS}
         itkHistogramToEntropyImageFilterTest1
         ${ITK_SOURCE_DIR}/Testing/Data/Input/cthead1.png
         ${TEMP}/itkHistogramToEntropyImageFilterTest1.mhd)
add_test(itkHistogramToEntropyImageFilterTest2 ${CXX_TESTS}
         itkHistogramToEntropyImageFilterTest2
         ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
         ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
         ${TEMP}/itkHistogramToEntropyImageFilterTest2.mhd)
add_test(itkHistogramToIntensityImageFilterTest1 ${CXX_TESTS}
         itkHistogramToIntensityImageFilterTest1
         ${ITK_SOURCE_DIR}/Testing/Data/Input/cthead1.png
         ${TEMP}/itkHistogramToIntensityImageFilterTest1.mhd)
add_test(itkHistogramToIntensityImageFilterTest2 ${CXX_TESTS}
         itkHistogramToIntensityImageFilterTest2
         ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
         ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
         ${TEMP}/itkHistogramToIntensityImageFilterTest2.mhd)
add_test(itkHistogramToLogProbabilityImageFilterTest1 ${CXX_TESTS}
         itkHistogramToLogProbabilityImageFilterTest1
         ${ITK_SOURCE_DIR}/Testing/Data/Input/cthead1.png
         ${TEMP}/itkHistogramToLogProbabilityImageFilterTest1.mhd)
add_test(itkHistogramToLogProbabilityImageFilterTest2 ${CXX_TESTS}
         itkHistogramToLogProbabilityImageFilterTest2
         ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
         ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
         ${TEMP}/itkHistogramToLogProbabilityImageFilterTest2.mhd)
add_test(itkHistogramToProbabilityImageFilterTest1 ${CXX_TESTS}
         itkHistogramToProbabilityImageFilterTest1
         ${ITK_SOURCE_DIR}/Testing/Data/Input/cthead1.png
         ${TEMP}/itkHistogramToProbabilityImageFilterTest1.mhd)
add_test(itkHistogramToProbabilityImageFilterTest2 ${CXX_TESTS}
         itkHistogramToProbabilityImageFilterTest2
         ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
         ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
         ${TEMP}/itkHistogramToProbabilityImageFilterTest2.mhd)
add_test(itkImageToHistogramFilterTest ${CXX_TESTS}
         itkImageToHistogramFilterTest)
add_test(itkImageToHistogramFilterTest2 ${CXX_TESTS}
         itkImageToHistogramFilterTest2
         ${ITK_SOURCE_DIR}/Testing/Data/Input/VisibleWomanEyeSlice.png
         ${TEMP}/itkImageToHistogramFilterTest2.txt)
add_test(itkImageToHistogramFilterTest3 ${CXX_TESTS}
         itkImageToHistogramFilterTest3
         ${ITK_SOURCE_DIR}/Testing/Data/Input/cthead1.png
         ${TEMP}/itkImageToHistogramFilterTest3.txt)
add_test(itkImageToListSampleFilterTest ${CXX_TESTS}
         itkImageToListSampleFilterTest)
add_test(itkImageToListSampleFilterTest2 ${CXX_TESTS}
         itkImageToListSampleFilterTest2)
add_test(itkImageToListSampleFilterTest3 ${CXX_TESTS}
         itkImageToListSampleFilterTest3)
add_test(itkListSampleTest1 ${CXX_TESTS}
         itkListSampleTest 1)
add_test(itkListSampleTest2 ${CXX_TESTS}
         itkListSampleTest 5)
add_test(itkListSampleTest3 ${CXX_TESTS}
         itkListSampleTest 10)
add_test(itkListSampleTest4 ${CXX_TESTS}
         itkListSampleTest 100)
add_test(itkSampleToHistogramFilterTest ${CXX_TESTS}
         itkSampleToHistogramFilterTest)
add_test(itkSampleToHistogramFilterTest2 ${CXX_TESTS}
         itkSampleToHistogramFilterTest2)
add_test(itkSampleToHistogramFilterTest3 ${CXX_TESTS}
         itkSampleToHistogramFilterTest3)
add_test(itkSampleToHistogramFilterTest4 ${CXX_TESTS}
         itkSampleToHistogramFilterTest4)
add_test(itkSampleToHistogramFilterTest5 ${CXX_TESTS}
         itkSampleToHistogramFilterTest5
         ${ITK_SOURCE_DIR}/Testing/Data/Input/VisibleWomanEyeSlice.png)
add_test(itkSampleToHistogramFilterTest6 ${CXX_TESTS}
         itkSampleToHistogramFilterTest6)
add_test(itkSampleToHistogramFilterTest7 ${CXX_TESTS}
         itkSampleToHistogramFilterTest7)
add_test(itkStandardDeviationPerComponentSampleFilterTest ${CXX_TESTS}
         itkStandardDeviationPerComponentSampleFilterTest)
add_test(itkMeanSampleFilterTest ${CXX_TESTS}
         itkMeanSampleFilterTest)
add_test(itkMeanSampleFilterTest2 ${CXX_TESTS}
         itkMeanSampleFilterTest2)
add_test(itkMeanSampleFilterTest3 ${CXX_TESTS}
         itkMeanSampleFilterTest3)
add_test(itkMeasurementVectorTraitsTest ${CXX_TESTS}
         itkMeasurementVectorTraitsTest)
add_test(itkMembershipSampleTest1 ${CXX_TESTS}
         itkMembershipSampleTest1)
add_test(itkMembershipSampleTest2 ${CXX_TESTS}
         itkMembershipSampleTest2)
add_test(itkMembershipSampleTest3 ${CXX_TESTS}
         itkMembershipSampleTest3)
add_test(itkMembershipSampleTest4 ${CXX_TESTS}
         itkMembershipSampleTest4)
add_test(itkMixtureModelComponentBaseTest ${CXX_TESTS}
         itkMixtureModelComponentBaseTest)
add_test(itkNeighborhoodSamplerTest1 ${CXX_TESTS}
         itkNeighborhoodSamplerTest1)
add_test(itkNormalVariateGeneratorTest1 ${CXX_TESTS}
         itkNormalVariateGeneratorTest1)
add_test(itkRandomVariateGeneratorBaseTest ${CXX_TESTS}
         itkRandomVariateGeneratorBaseTest)
add_test(itkScalarImageToCooccurrenceMatrixFilterTest ${CXX_TESTS}
         itkScalarImageToCooccurrenceMatrixFilterTest)
add_test(itkScalarImageToCooccurrenceMatrixFilterTest2 ${CXX_TESTS}
         itkScalarImageToCooccurrenceMatrixFilterTest2)
add_test(itkScalarImageToCooccurrenceListSampleFilterTest ${CXX_TESTS}
         itkScalarImageToCooccurrenceListSampleFilterTest)
add_test(itkSampleTest ${CXX_TESTS}
         itkSampleTest)
add_test(itkSampleTest2 ${CXX_TESTS}
         itkSampleTest2)
add_test(itkSampleTest3 ${CXX_TESTS}
         itkSampleTest3)
add_test(itkSampleTest4 ${CXX_TESTS}
         itkSampleTest4)
add_test(itkSampleClassifierFilterTest1 ${CXX_TESTS}
         itkSampleClassifierFilterTest1)
add_test(itkSampleClassifierFilterTest2 ${CXX_TESTS}
         itkSampleClassifierFilterTest2)
add_test(itkSampleClassifierFilterTest3 ${CXX_TESTS}
         itkSampleClassifierFilterTest3)
add_test(itkSampleClassifierFilterTest4 ${CXX_TESTS}
         itkSampleClassifierFilterTest4)
add_test(itkSampleClassifierFilterTest5 ${CXX_TESTS}
         itkSampleClassifierFilterTest5)
add_test(itkSampleClassifierFilterTest6 ${CXX_TESTS}
         itkSampleClassifierFilterTest6)
add_test(itkSampleClassifierFilterTest7 ${CXX_TESTS}
         itkSampleClassifierFilterTest7
         ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat
         ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussianTesting.dat )

add_test(itkSampleToSubsampleFilterTest1 ${CXX_TESTS}
         itkSampleToSubsampleFilterTest1)
add_test(itkSubsampleTest ${CXX_TESTS}
         itkSubsampleTest)
add_test(itkSubsampleTest2 ${CXX_TESTS}
         itkSubsampleTest2)
add_test(itkSubsampleTest3 ${CXX_TESTS}
         itkSubsampleTest3)
add_test(itkStatisticsAlgorithmTest ${CXX_TESTS}
         itkStatisticsAlgorithmTest)
add_test(itkStatisticsAlgorithmTest2 ${CXX_TESTS}
         itkStatisticsAlgorithmTest2)
add_test(itkWeightedMeanSampleFilterTest ${CXX_TESTS}
         itkWeightedMeanSampleFilterTest)
add_test(itkWeightedCovarianceSampleFilterTest ${CXX_TESTS}
         itkWeightedCovarianceSampleFilterTest)
add_test(itkWeightedCovarianceSampleFilterTest2 ${CXX_TESTS}
         itkWeightedCovarianceSampleFilterTest2)
add_test(itkImageToListSampleAdaptorTest ${CXX_TESTS}
         itkImageToListSampleAdaptorTest)
add_test(itkImageToListSampleAdaptorTest2 ${CXX_TESTS}
         itkImageToListSampleAdaptorTest2)
add_test(itkJointDomainImageToListSampleAdaptorTest ${CXX_TESTS}
         itkJointDomainImageToListSampleAdaptorTest)
add_test(itkPointSetToListSampleAdaptorTest ${CXX_TESTS}
         itkPointSetToListSampleAdaptorTest)
add_test(itkProbabilityDistributionTest ${CXX_TESTS}
         itkProbabilityDistributionTest)
add_test(itkDenseFrequencyContainer2Test ${CXX_TESTS}
         itkDenseFrequencyContainer2Test)
add_test(itkSparseFrequencyContainer2Test ${CXX_TESTS}
         itkSparseFrequencyContainer2Test)
add_test(itkScalarImageToTextureFeaturesFilterTest ${CXX_TESTS} itkScalarImageToTextureFeaturesFilterTest)


add_test(itkMembershipFunctionBaseTest ${CXX_TESTS} itkMembershipFunctionBaseTest)
add_test(itkMembershipFunctionBaseTest2 ${CXX_TESTS} itkMembershipFunctionBaseTest2)
add_test(itkDistanceToCentroidMembershipFunctionTest ${CXX_TESTS} itkDistanceToCentroidMembershipFunctionTest)
add_test(itkGaussianMembershipFunctionTest ${CXX_TESTS}
itkGaussianMembershipFunctionTest)
add_test(itkDistanceMetricTest ${CXX_TESTS} itkDistanceMetricTest)
add_test(itkDistanceMetricTest2 ${CXX_TESTS} itkDistanceMetricTest2)
add_test(itkEuclideanDistanceMetricTest ${CXX_TESTS} itkEuclideanDistanceMetricTest)
add_test(itkEuclideanSquareDistanceMetricTest ${CXX_TESTS} itkEuclideanSquareDistanceMetricTest)
add_test(itkMahalanobisDistanceMetricTest ${CXX_TESTS} itkMahalanobisDistanceMetricTest)
add_test(itkManhattanDistanceMetricTest ${CXX_TESTS} itkManhattanDistanceMetricTest)
add_test(itkDecisionRuleTest ${CXX_TESTS} itkDecisionRuleTest)
add_test(itkMaximumDecisionRuleTest ${CXX_TESTS} itkMaximumDecisionRuleTest)
add_test(itkMaximumRatioDecisionRuleTest ${CXX_TESTS} itkMaximumRatioDecisionRuleTest)
add_test(itkMinimumDecisionRuleTest ${CXX_TESTS} itkMinimumDecisionRuleTest)

add_test(itkKdTreeBasedKmeansEstimatorTest ${CXX_TESTS}
         itkKdTreeBasedKmeansEstimatorTest
         ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat
         1 28.54746 0.07)
add_test(itkKdTreeGeneratorTest ${CXX_TESTS}
         itkKdTreeGeneratorTest ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)
add_test(itkKdTreeTest1 ${CXX_TESTS} itkKdTreeTest1 100 100 1 ${TEMP}/itkKdTreeTest1_100.dot)
add_test(itkKdTreeTestSamplePoints ${CXX_TESTS} itkKdTreeTestSamplePoints )
add_test(itkKdTreeTest2 ${CXX_TESTS} itkKdTreeTest2
         ${ITK_DATA_ROOT}/Input/pointsForKdTree.txt 1 ${TEMP}/itkKdTreeTest2.dot)
add_test(itkKdTreeTest3 ${CXX_TESTS} itkKdTreeTest2
         ${ITK_DATA_ROOT}/Input/pointsForKdTree2.txt 1 ${TEMP}/itkKdTreeTest3.dot)
if(DOXYGEN_DOT_EXECUTABLE)
add_test(itkKdTreeTest1Plot ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${TEMP}/itkKdTreeTest1_100.png ${TEMP}/itkKdTreeTest1_100.dot)
set_tests_properties( itkKdTreeTest1Plot PROPERTIES DEPENDS itkKdTreeTest1 )
add_test(itkKdTreeTest2Plot ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${TEMP}/itkKdTreeTest2.png ${TEMP}/itkKdTreeTest2.dot)
set_tests_properties( itkKdTreeTest2Plot PROPERTIES DEPENDS itkKdTreeTest2 )
add_test(itkKdTreeTest3Plot ${DOXYGEN_DOT_EXECUTABLE} -Tpng -o  ${TEMP}/itkKdTreeTest3.png ${TEMP}/itkKdTreeTest3.dot)
set_tests_properties( itkKdTreeTest3Plot PROPERTIES DEPENDS itkKdTreeTest3 )
endif(DOXYGEN_DOT_EXECUTABLE)
add_test(itkKdTreeTest4 ${CXX_TESTS} itkKdTreeTest1 1000 1000 1 )
add_test(itkKdTreeTest5 ${CXX_TESTS} itkKdTreeTest1 1000 1000 2 )
add_test(itkKdTreeTest6 ${CXX_TESTS} itkKdTreeTest1 1000 1000 3 )
add_test(itkKdTreeTest7 ${CXX_TESTS} itkKdTreeTest1 1000 1000 5 )
add_test(itkKdTreeTest7 ${CXX_TESTS} itkKdTreeTest1 1000 1000 10 )
add_test(itkKdTreeTest8 ${CXX_TESTS} itkKdTreeTest1 1000 1000 20 )
add_test(itkKdTreeTest9 ${CXX_TESTS} itkKdTreeTest1 1000 1000 100 )
add_test(itkWeightedCentroidKdTreeGeneratorTest1 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 1 )
add_test(itkWeightedCentroidKdTreeGeneratorTest2 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 2 )
add_test(itkWeightedCentroidKdTreeGeneratorTest3 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 3 )
add_test(itkWeightedCentroidKdTreeGeneratorTest4 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 5 )
add_test(itkWeightedCentroidKdTreeGeneratorTest5 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 10 )
add_test(itkWeightedCentroidKdTreeGeneratorTest6 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 20 )
add_test(itkWeightedCentroidKdTreeGeneratorTest7 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest1 1000 1000 100 )
add_test(itkWeightedCentroidKdTreeGeneratorTest8 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest8 1000 1000 1 )
add_test(itkWeightedCentroidKdTreeGeneratorTest9 ${CXX_TESTS}
  itkWeightedCentroidKdTreeGeneratorTest9 1000 1000 1 )

add_test(itkGaussianMixtureModelComponentTest
         ${CXX_TESTS}
         itkGaussianMixtureModelComponentTest ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)

add_test(itkExpectationMaximizationMixtureModelEstimatorTest
         ${CXX_TESTS}
         itkExpectationMaximizationMixtureModelEstimatorTest ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat)

add_test(itkImageClassifierFilterTest
         ${CXX_TESTS}
        --compare ${ITK_DATA_ROOT}/Baseline/Statistics/ImageClassifierFilterTestClassifiedImageTest.png
         ${TEMP}/ImageClassifierFilterTestClassifiedImage.png
         itkImageClassifierFilterTest
         ${TEMP}/ImageClassifierFilterTestClassifiedImage.png)

add_test(itkChiSquareDistributionTest ${CXX_TESTS}
  itkChiSquareDistributionTest
  )
add_test(itkGaussianDistributionTest ${CXX_TESTS}
  itkGaussianDistributionTest
  )
add_test(itkTDistributionTest ${CXX_TESTS} itkTDistributionTest)

endif( NOT ITK_DISABLE_CXX_TESTING )
