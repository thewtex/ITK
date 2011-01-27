if( NOT ITK_DISABLE_CXX_TESTING )
if(ITK_BINARY_DIR)
  if(TCL_TCLSH)
    add_test(PrintSelf-Algorithms ${TCL_TCLSH}
             ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl
             ${ITK_SOURCE_DIR}/Code/Algorithms)
  endif(TCL_TCLSH)
endif(ITK_BINARY_DIR)

set(BASELINE ${ITK_DATA_ROOT}/Baseline/Algorithms)
set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(TEMP ${ITKTesting_BINARY_DIR}/Temporary)
if(USE_FFTWD)
  set(CURVATUREREGISTRATION_SRCS itkCurvatureRegistrationFilterTest.cxx)
endif(USE_FFTWD)

set(KIT Algorithms)
set(CXX_TESTS ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)

set(KIT_LIBS ITKAlgorithms ITKFEM ITKNumerics ITKStatistics)
set(KitTests
${CURVATUREREGISTRATION_SRCS}
itk2DDeformableTest.cxx
itkAnisotropicFourthOrderLevelSetImageFilterTest.cxx
itkAntiAliasBinaryImageFilterTest.cxx
itkAutomaticTopologyMeshSourceTest.cxx
itkAutomaticTopologyQuadEdgeMeshSourceTest.cxx
itkBayesianClassifierImageFilterTest.cxx
itkBinaryMask3DMeshSourceTest.cxx
itkBinaryMask3DQuadEdgeMeshSourceTest.cxx
itkBinaryMedialNodeMetricTest.cxx
itkBinaryMinMaxCurvatureFlowImageFilterTest.cxx
itkBinaryThinningImageFilterTest.cxx
itkBioCellTest.cxx
itkBioCellularAggregateTest.cxx
itkBioGeneNetworkTest.cxx
itkBioGeneTest.cxx
itkBioGenomeTest.cxx
itkBresenhamLineTest.cxx
itkCannySegmentationLevelSetImageFilterTest.cxx
itkCollidingFrontsImageFilterTest.cxx
itkCompareHistogramImageToImageMetricTest.cxx
itkCorrelationCoefficientHistogramImageToImageMetricTest.cxx
itkCurvatureFlowTest.cxx
itkCurvesLevelSetImageFilterTest.cxx
itkCurvesLevelSetImageFilterZeroSigmaTest.cxx
itkDeformableSimplexMesh3DBalloonForceFilterTest.cxx
itkDeformableSimplexMesh3DFilterTest.cxx
itkDeformableSimplexMesh3DGradientConstraintForceFilterTest.cxx
itkDeformableTest.cxx
itkDemonsRegistrationFilterTest.cxx
itkExtensionVelocitiesImageFilterTest.cxx
itkExtractMeshConnectedRegionsTest.cxx
itkFEMRegistrationFilterTest.cxx
itkFFTTest.cxx
itkFastChamferDistanceImageFilterTest.cxx
itkFastMarchingExtensionImageFilterTest.cxx
itkFastMarchingTest.cxx
itkFastMarchingTest2.cxx
itkFastMarchingUpwindGradientTest.cxx
itkGeodesicActiveContourLevelSetImageFilterTest.cxx
itkGeodesicActiveContourLevelSetImageFilterZeroSigmaTest.cxx
itkGeodesicActiveContourShapePriorLevelSetImageFilterTest.cxx
itkGeodesicActiveContourShapePriorLevelSetImageFilterTest_2.cxx
itkGibbsTest.cxx
itkGradientDifferenceImageToImageMetricTest.cxx
itkGradientVectorFlowImageFilterTest.cxx
itkHistogramImageToImageMetricTest.cxx
itkHistogramMatchingImageFilterTest.cxx
itkImageMomentsTest.cxx
itkImagePCADecompositionCalculatorTest.cxx
itkImagePCAShapeModelEstimatorTest.cxx
itkImageRegistrationMethodTest.cxx
itkImageRegistrationMethodTest_1.cxx
itkImageRegistrationMethodTest_10.cxx
itkImageRegistrationMethodTest_11.cxx
itkImageRegistrationMethodTest_12.cxx
itkImageRegistrationMethodTest_13.cxx
itkImageRegistrationMethodTest_14.cxx
itkImageRegistrationMethodTest_15.cxx
itkImageRegistrationMethodTest_16.cxx
itkImageRegistrationMethodTest_2.cxx
itkImageRegistrationMethodTest_3.cxx
itkImageRegistrationMethodTest_4.cxx
itkImageRegistrationMethodTest_5.cxx
itkImageRegistrationMethodTest_6.cxx
itkImageRegistrationMethodTest_7.cxx
itkImageRegistrationMethodTest_8.cxx
itkImageRegistrationMethodTest_9.cxx
itkInterpolateTest.cxx
itkIsoContourDistanceImageFilterTest.cxx
itkIsolatedWatershedImageFilterTest.cxx
itkIsotropicFourthOrderLevelSetImageFilterTest.cxx
itkKalmanLinearEstimatorTest.cxx
itkKappaStatisticImageToImageMetricTest.cxx
itkKmeansModelEstimatorTest.cxx
itkKullbackLeiblerCompareHistogramImageToImageMetricTest.cxx
itkLabelVotingImageFilterTest.cxx
itkLaplacianSegmentationLevelSetImageFilterTest.cxx
itkLevelSetMotionRegistrationFilterTest
itkLevelSetNeighborhoodExtractorTest.cxx
itkLevelSetVelocityNeighborhoodExtractorTest.cxx
itkMIRegistrationFunctionTest.cxx
itkMRFImageFilterTest.cxx
itkMRIBiasFieldCorrectionFilterTest.cxx
itkMatchCardinalityImageToImageMetricTest.cxx
itkMattesMutualInformationImageToImageMetricTest.cxx
itkMeanReciprocalSquareDifferenceImageMetricTest.cxx
itkMeanReciprocalSquareDifferencePointSetToImageMetricTest.cxx
itkMeanSquaresHistogramImageToImageMetricTest.cxx
itkMeanSquaresImageMetricTest.cxx
itkMeanSquaresPointSetToImageMetricTest.cxx
itkMinMaxCurvatureFlowImageFilterTest.cxx
itkMultiResolutionImageRegistrationMethodTest.cxx
itkMultiResolutionImageRegistrationMethodTest_1.cxx
itkMultiResolutionImageRegistrationMethodTest_2.cxx
itkMultiResolutionPDEDeformableRegistrationTest.cxx
itkMultiResolutionPyramidImageFilterTest.cxx
itkMutualInformationHistogramImageToImageMetricTest.cxx
itkMutualInformationMetricTest.cxx
itkNarrowBandCurvesLevelSetImageFilterTest.cxx
itkNarrowBandThresholdSegmentationLevelSetImageFilterTest.cxx
itkNewTest.cxx
itkNormalizedCorrelationImageMetricTest.cxx
itkNormalizedCorrelationPointSetToImageMetricTest.cxx
itkNormalizedMutualInformationHistogramImageToImageMetricTest.cxx
itkOrthogonalSwath2DPathFilterTest.cxx
itkOtsuMultipleThresholdsCalculatorTest.cxx
itkOtsuMultipleThresholdsImageFilterTest.cxx
itkOtsuThresholdImageCalculatorTest.cxx
itkOtsuThresholdImageFilterTest.cxx
itkPointSetToImageRegistrationTest_1.cxx
itkPointSetToPointSetRegistrationTest.cxx
itkPointSetToSpatialObjectDemonsRegistrationTest.cxx
itkQuadEdgeMeshCleanFilterTest.cxx
itkQuadEdgeMeshDelaunayConformingFilterTest.cxx
itkQuadEdgeMeshGaussianCurvatureTest.cxx
itkQuadEdgeMeshLinearParameterizationTest.cxx
itkQuadEdgeMeshMaxCurvatureTest.cxx
itkQuadEdgeMeshMeanCurvatureTest.cxx
itkQuadEdgeMeshMinCurvatureTest.cxx
itkQuadEdgeMeshNormalFilterTest.cxx
itkQuadEdgeMeshQuadricDecimationTest.cxx
itkQuadEdgeMeshSmoothingTest.cxx
itkQuadEdgeMeshSquaredEdgeLengthDecimationTest.cxx
itkRayCastInterpolateImageFunctionTest.cxx
itkRecursiveMultiResolutionPyramidImageFilterTest.cxx
itkRegionGrow2DTest.cxx
itkRegularSphereMeshSourceTest.cxx
itkRegularSphereQuadEdgeMeshSourceTest.cxx
itkReinitializeLevelSetImageFilterTest.cxx
itkSTAPLEImageFilterTest.cxx
itkScalarImageKmeansImageFilter3DTest.cxx
itkShapeDetectionLevelSetImageFilterTest.cxx
itkShapePriorMAPCostFunctionTest.cxx
itkShapePriorSegmentationLevelSetFunctionTest.cxx
itkSimplexMeshVolumeCalculatorTest.cxx
itkSpatialObjectToImageRegistrationTest.cxx
itkSphereMeshSourceTest.cxx
itkSupervisedImageClassifierTest.cxx
itkSymmetricForcesDemonsRegistrationFilterTest.cxx
itkThresholdMaximumConnectedComponentsImageFilterTest.cxx
itkThresholdSegmentationLevelSetImageFilterTest.cxx
itkUnsharpMaskLevelSetImageFilterTest.cxx
itkVectorThresholdSegmentationLevelSetImageFilterTest.cxx
itkVoronoiDiagram2DTest.cxx
itkVoronoiPartitioningImageFilterTest.cxx
itkVoronoiSegmentationImageFilterTest.cxx
itkVoronoiSegmentationRGBImageFilterTest.cxx
itkWatershedImageFilterTest.cxx
)

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkv4TestMainAfter.inc\"")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KitTests}
  EXTRA_INCLUDE itkv4TestMain.h
  FUNCTION ProcessArguments
  )

add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT_LIBS})

set(ALGORITHMS_PRINT_TEST ${CXX_TEST_PATH}/itkAlgorithmsPrintTest)
set(ALGORITHMS_PRINT_TEST2 ${CXX_TEST_PATH}/itkAlgorithmsPrintTest2)
set(ALGORITHMS_PRINT_TEST3 ${CXX_TEST_PATH}/itkAlgorithmsPrintTest3)
set(ALGORITHMS_PRINT_TEST4 ${CXX_TEST_PATH}/itkAlgorithmsPrintTest4)

set(ALGORITHMS_HEADER_TESTS ${CXX_TEST_PATH}/itkAlgorithmsHeaderTest)

add_test(itkAlgorithmsPrintTest ${ALGORITHMS_PRINT_TEST})
add_test(itkAlgorithmsPrintTest2 ${ALGORITHMS_PRINT_TEST2})
add_test(itkAlgorithmsPrintTest3 ${ALGORITHMS_PRINT_TEST3})
add_test(itkAlgorithmsPrintTest4 ${ALGORITHMS_PRINT_TEST4})

add_test(itkAnisotropicFourthOrderLevelSetImageFilterTest ${CXX_TESTS} itkAnisotropicFourthOrderLevelSetImageFilterTest)
add_test(itkAntiAliasBinaryImageFilterTest ${CXX_TESTS} itkAntiAliasBinaryImageFilterTest)
add_test(itkAutomaticTopologyMeshSourceTest ${CXX_TESTS} itkAutomaticTopologyMeshSourceTest)
add_test(itkBinaryMinMaxCurvatureFlowImageFilterTest ${CXX_TESTS} itkBinaryMinMaxCurvatureFlowImageFilterTest )
add_test(itkBinaryMask3DMeshSourceTest ${CXX_TESTS} itkBinaryMask3DMeshSourceTest )
add_test(itkBioCellTest ${CXX_TESTS} itkBioCellTest )
add_test(itkBioCellularAggregateTest ${CXX_TESTS} itkBioCellularAggregateTest )
add_test(itkBioGeneTest ${CXX_TESTS} itkBioGeneTest )
add_test(itkBioGenomeTest ${CXX_TESTS} itkBioGenomeTest )
add_test(itkBioGeneNetworkTest ${CXX_TESTS} itkBioGeneNetworkTest )
add_test(itkBresenhamLineTest ${CXX_TESTS} itkBresenhamLineTest )
add_test(itkCannySegmentationLevelSetImageFilterTest ${CXX_TESTS} itkCannySegmentationLevelSetImageFilterTest)
add_test(itkCorrelationCoefficientHistogramImageToImageMetricTest ${CXX_TESTS} itkCorrelationCoefficientHistogramImageToImageMetricTest)
add_test(itkCompareHistogramImageToImageMetricTest ${CXX_TESTS} itkCompareHistogramImageToImageMetricTest)
add_test(itkCurvesLevelSetImageFilterTest ${CXX_TESTS} itkCurvesLevelSetImageFilterTest)
add_test(itkDemonsRegistrationFilterTest ${CXX_TESTS} itkDemonsRegistrationFilterTest)
add_test(itkExtensionVelocitiesImageFilterTest ${CXX_TESTS} itkExtensionVelocitiesImageFilterTest)
add_test(itkExtractMeshConnectedRegionsTest ${CXX_TESTS} itkExtractMeshConnectedRegionsTest)
add_test(itkFastChamferDistanceImageFilterTest ${CXX_TESTS} itkFastChamferDistanceImageFilterTest)
add_test(itkFastMarchingTest ${CXX_TESTS} itkFastMarchingTest)
add_test(itkFastMarchingTest2 ${CXX_TESTS} itkFastMarchingTest2)
add_test(itkFastMarchingExtensionImageFilterTest ${CXX_TESTS} itkFastMarchingExtensionImageFilterTest)
add_test(itkFastMarchingUpwindGradientTest ${CXX_TESTS} itkFastMarchingUpwindGradientTest)

# This test uses inputs from BayesianClassifierInitializer.
# If that output changes, a new file should be placed in Examples/Data.
add_test(itkBayesianClassifierImageFilterTest ${CXX_TESTS}
  --compare ${BASELINE}/itkBayesianClassifierImageFilterTestLabelMap.png
            ${TEMP}/itkBayesianClassifierImageFilterTestLabelMap.png
  itkBayesianClassifierImageFilterTest
     ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
     ${TEMP}/itkBayesianClassifierImageFilterTestLabelMap.png
     4  # Number of classes
     2  # Number of smoothing iterations
)

add_test(itkCollidingFrontsImageFilterTest ${CXX_TESTS} itkCollidingFrontsImageFilterTest)
add_test(itkCurvesLevelSetImageFilterZeroSigmaTest ${CXX_TESTS} itkCurvesLevelSetImageFilterZeroSigmaTest)
add_test(itkGeodesicActiveContourLevelSetImageFilterZeroSigmaTest ${CXX_TESTS} itkGeodesicActiveContourLevelSetImageFilterZeroSigmaTest)
add_test(itkLabelVotingImageFilterTest ${CXX_TESTS} itkLabelVotingImageFilterTest)
add_test(itkBinaryMask3DQuadEdgeMeshSourceTest ${CXX_TESTS} itkBinaryMask3DQuadEdgeMeshSourceTest)
add_test(itkAutomaticTopologyQuadEdgeMeshSourceTest ${CXX_TESTS} itkAutomaticTopologyQuadEdgeMeshSourceTest)

add_test(itkLevelSetMotionRegistrationFilterTest ${CXX_TESTS}
  itkLevelSetMotionRegistrationFilterTest
  ${TEMP}/itkLevelSetMotionRegistrationFilterTestFixedImage.mha
  ${TEMP}/itkLevelSetMotionRegistrationFilterTestMovingImage.mha
  ${TEMP}/itkLevelSetMotionRegistrationFilterTestResampledImage.mha
)

add_test(itkFEMRegistrationFilterTest ${CXX_TESTS} itkFEMRegistrationFilterTest)
add_test(itkGeodesicActiveContourLevelSetImageFilterTest ${CXX_TESTS} itkGeodesicActiveContourLevelSetImageFilterTest)
add_test(itkGeodesicActiveContourShapePriorLevelSetImageFilterTest ${CXX_TESTS} itkGeodesicActiveContourShapePriorLevelSetImageFilterTest)
add_test(itkGeodesicActiveContourShapePriorLevelSetImageFilterTest_2 ${CXX_TESTS} itkGeodesicActiveContourShapePriorLevelSetImageFilterTest_2)
add_test(itkGradientVectorFlowImageFilterTest ${CXX_TESTS} itkGradientVectorFlowImageFilterTest)
add_test(itkGradientDifferenceImageToImageMetricTest ${CXX_TESTS} itkGradientDifferenceImageToImageMetricTest)
add_test(itkHistogramMatchingImageFilterTest ${CXX_TESTS} itkHistogramMatchingImageFilterTest)
add_test(itkHistogramImageToImageMetricTest ${CXX_TESTS} itkHistogramImageToImageMetricTest)
add_test(itkImageMomentsTest ${CXX_TESTS} itkImageMomentsTest)
add_test(itkImagePCAShapeModelEstimatorTest ${CXX_TESTS} itkImagePCAShapeModelEstimatorTest)
add_test(itkImagePCADecompositionCalculatorTest ${CXX_TESTS} itkImagePCADecompositionCalculatorTest)
add_test(itkImageRegistrationMethodTest ${CXX_TESTS}   itkImageRegistrationMethodTest  )
add_test(itkImageRegistrationMethodTest_1 ${CXX_TESTS} itkImageRegistrationMethodTest_1)
add_test(itkImageRegistrationMethodTest_2 ${CXX_TESTS} itkImageRegistrationMethodTest_2)
add_test(itkImageRegistrationMethodTest_3 ${CXX_TESTS} itkImageRegistrationMethodTest_3)
add_test(itkImageRegistrationMethodTest_4 ${CXX_TESTS} itkImageRegistrationMethodTest_4)
add_test(itkImageRegistrationMethodTest_5 ${CXX_TESTS} itkImageRegistrationMethodTest_5)
add_test(itkImageRegistrationMethodTest_6 ${CXX_TESTS} itkImageRegistrationMethodTest_6)
add_test(itkImageRegistrationMethodTest_7 ${CXX_TESTS} itkImageRegistrationMethodTest_7)
add_test(itkImageRegistrationMethodTest_8 ${CXX_TESTS} itkImageRegistrationMethodTest_8)
add_test(itkImageRegistrationMethodTest_9 ${CXX_TESTS} itkImageRegistrationMethodTest_9)
add_test(itkImageRegistrationMethodTest_10 ${CXX_TESTS} itkImageRegistrationMethodTest_10)
add_test(itkImageRegistrationMethodTest_11 ${CXX_TESTS} itkImageRegistrationMethodTest_11)
add_test(itkImageRegistrationMethodTest_12 ${CXX_TESTS} itkImageRegistrationMethodTest_12)
add_test(itkImageRegistrationMethodTest_13 ${CXX_TESTS} itkImageRegistrationMethodTest_13)
add_test(itkImageRegistrationMethodTest_14 ${CXX_TESTS} itkImageRegistrationMethodTest_14)
add_test(itkImageRegistrationMethodTest_15 ${CXX_TESTS} itkImageRegistrationMethodTest_15)
add_test(itkImageRegistrationMethodTest_16 ${CXX_TESTS} itkImageRegistrationMethodTest_16)
add_test(itkInterpolateTest ${CXX_TESTS} itkInterpolateTest)
add_test(itkIsoContourDistanceImageFilterTest ${CXX_TESTS} itkIsoContourDistanceImageFilterTest)
add_test(itkIsotropicFourthOrderLevelSetImageFilterTest ${CXX_TESTS} itkIsotropicFourthOrderLevelSetImageFilterTest)
add_test(itkKalmanLinearEstimatorTest ${CXX_TESTS} itkKalmanLinearEstimatorTest)
add_test(itkKmeansModelEstimatorTest ${CXX_TESTS} itkKmeansModelEstimatorTest)
add_test(itkScalarImageKmeansImageFilter3DTest ${CXX_TESTS}
    --compare ${ITK_SOURCE_DIR}/Examples/Data/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
            ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
  itkScalarImageKmeansImageFilter3DTest ${ITK_SOURCE_DIR}/Examples/Data/KmeansTest_T1UCharRaw.nii.gz
  ${ITK_SOURCE_DIR}/Examples/Data/KmeansTest_T1RawSkullStrip.nii.gz ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz )
add_test(itkKullbackLeiblerCompareHistogramImageToImageMetricTest ${CXX_TESTS} itkKullbackLeiblerCompareHistogramImageToImageMetricTest)
add_test(itkLaplacianSegmentationLevelSetImageFilterTest ${CXX_TESTS} itkLaplacianSegmentationLevelSetImageFilterTest)
add_test(itkLevelSetNeighborhoodExtractorTest ${CXX_TESTS} itkLevelSetNeighborhoodExtractorTest)
add_test(itkLevelSetVelocityNeighborhoodExtractorTest ${CXX_TESTS} itkLevelSetVelocityNeighborhoodExtractorTest)

add_test(itkMattesMutualInformationImageToImageMetricTest
  ${CXX_TESTS} itkMattesMutualInformationImageToImageMetricTest 1 1)

add_test(itkMattesMutualInformationImageToImageMetricTest2
  ${CXX_TESTS} itkMattesMutualInformationImageToImageMetricTest 1 0)

add_test(itkMattesMutualInformationImageToImageMetricTest3
  ${CXX_TESTS} itkMattesMutualInformationImageToImageMetricTest 0 1)

add_test(itkMattesMutualInformationImageToImageMetricTest4
  ${CXX_TESTS} itkMattesMutualInformationImageToImageMetricTest 0 0)

add_test(itkMeanSquaresImageMetricTest ${CXX_TESTS} itkMeanSquaresImageMetricTest)
add_test(itkMeanSquaresHistogramImageToImageMetricTest ${CXX_TESTS} itkMeanSquaresHistogramImageToImageMetricTest)
add_test(itkMinMaxCurvatureFlowImageFilterTest ${CXX_TESTS} itkMinMaxCurvatureFlowImageFilterTest)
add_test(itkMRFImageFilterTest ${CXX_TESTS} itkMRFImageFilterTest)
add_test(itkMRIBiasFieldCorrectionFilterTest ${CXX_TESTS} itkMRIBiasFieldCorrectionFilterTest)
add_test(itkMultiResolutionPyramidImageFilterWithResampleFilterTest ${CXX_TESTS} itkMultiResolutionPyramidImageFilterTest Resample)
add_test(itkMultiResolutionPyramidImageFilterWithShrinkFilterTest ${CXX_TESTS} itkMultiResolutionPyramidImageFilterTest Shrink)

add_test(itkRecursiveMultiResolutionPyramidImageFilterWithResampleFilterTest ${CXX_TESTS} itkRecursiveMultiResolutionPyramidImageFilterTest Resample)
add_test(itkRecursiveMultiResolutionPyramidImageFilterWithShrinkFilterTest ${CXX_TESTS} itkRecursiveMultiResolutionPyramidImageFilterTest Shrink)
add_test(itkRecursiveMultiResolutionPyramidImageFilterWithResampleFilterTest2 ${CXX_TESTS} itkMultiResolutionPyramidImageFilterTest Resample TestRecursive)
add_test(itkRecursiveMultiResolutionPyramidImageFilterWithShrinkFilterTest2 ${CXX_TESTS} itkMultiResolutionPyramidImageFilterTest Shrink TestRecursive)

add_test(itkMutualInformationHistogramImageToImageMetricTest ${CXX_TESTS} itkMutualInformationHistogramImageToImageMetricTest)
add_test(itkMultiResolutionPDEDeformableRegistrationTest ${CXX_TESTS}
  --compare ${BASELINE}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png
            ${TEMP}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png
  --compareNumberOfPixelsTolerance 20
  itkMultiResolutionPDEDeformableRegistrationTest
            ${TEMP}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png)
add_test(itkMultiResolutionImageRegistrationMethodTest ${CXX_TESTS}
itkMultiResolutionImageRegistrationMethodTest  )
add_test(itkMultiResolutionImageRegistrationMethodTest_1 ${CXX_TESTS}
itkMultiResolutionImageRegistrationMethodTest_1  )
add_test(itkMultiResolutionImageRegistrationMethodTest_2 ${CXX_TESTS}
itkMultiResolutionImageRegistrationMethodTest_2  )
add_test(itkMutualInformationMetricTest ${CXX_TESTS} itkMutualInformationMetricTest)
add_test(itkNarrowBandCurvesLevelSetImageFilterTest1 ${CXX_TESTS}
  --compare ${BASELINE}/itkNarrowBandCurvesLevelSetImageFilterTest.png
            ${TEMP}/itkNarrowBandCurvesLevelSetImageFilterTest.png
  itkNarrowBandCurvesLevelSetImageFilterTest
            ${TEMP}/itkNarrowBandCurvesLevelSetImageFilterTest.png)
add_test(itkNarrowBandThresholdSegmentationLevelSetImageFilterTest ${CXX_TESTS} itkNarrowBandThresholdSegmentationLevelSetImageFilterTest)
add_test(itkNewTest ${CXX_TESTS} itkNewTest)
add_test(itkNormalizedCorrelationImageMetricTest ${CXX_TESTS} itkNormalizedCorrelationImageMetricTest)
add_test(itkNormalizedCorrelationPointSetToImageMetricTest ${CXX_TESTS} itkNormalizedCorrelationPointSetToImageMetricTest)
add_test(itkMeanSquaresPointSetToImageMetricTest ${CXX_TESTS} itkMeanSquaresPointSetToImageMetricTest)
add_test(itkMeanReciprocalSquareDifferencePointSetToImageMetricTest ${CXX_TESTS} itkMeanReciprocalSquareDifferencePointSetToImageMetricTest)
add_test(itkNormalizedMutualInformationHistogramImageToImageMetricTest ${CXX_TESTS} itkNormalizedMutualInformationHistogramImageToImageMetricTest)
add_test(itkOtsuThresholdImageCalculatorTest ${CXX_TESTS} itkOtsuThresholdImageCalculatorTest)
add_test(itkOtsuMultipleThresholdsCalculatorTest ${CXX_TESTS} itkOtsuMultipleThresholdsCalculatorTest)
add_test(itkOrthogonalSwath2DPathFilterTest ${CXX_TESTS} itkOrthogonalSwath2DPathFilterTest)
add_test(itkMeanReciprocalSquareDifferenceImageMetricTest ${CXX_TESTS}  itkMeanReciprocalSquareDifferenceImageMetricTest)
add_test(itkPointSetToImageRegistrationTest_1 ${CXX_TESTS}  itkPointSetToImageRegistrationTest_1)
add_test(itkRegionGrow2DTest ${CXX_TESTS} itkRegionGrow2DTest)
add_test(itkReinitializeLevelSetImageFilterTest ${CXX_TESTS} itkReinitializeLevelSetImageFilterTest)
add_test(itkRegularSphereMeshSourceTest ${CXX_TESTS} itkRegularSphereMeshSourceTest)
add_test(itkRegularSphereQuadEdgeMeshSourceTest
  ${CXX_TESTS}
  itkRegularSphereQuadEdgeMeshSourceTest ${ITK_TEST_OUTPUT_DIR}/itkRegularSphereMeshQuadEdgeMeshSourceTest.vtk
  )
add_test(itkRayCastInterpolateImageFunctionTest ${CXX_TESTS} itkRayCastInterpolateImageFunctionTest)
add_test(itkSimplexMeshVolumeCalculatorTest ${CXX_TESTS} itkSimplexMeshVolumeCalculatorTest)
add_test(itkShapeDetectionLevelSetImageFilterTest ${CXX_TESTS} itkShapeDetectionLevelSetImageFilterTest)
add_test(itkShapePriorMAPCostFunctionTest ${CXX_TESTS} itkShapePriorMAPCostFunctionTest)
add_test(itkShapePriorSegmentationLevelSetFunctionTest ${CXX_TESTS} itkShapePriorSegmentationLevelSetFunctionTest)
add_test(itkSpatialObjectToImageRegistrationTest ${CXX_TESTS} itkSpatialObjectToImageRegistrationTest)
add_test(itkSupervisedImageClassifierTest ${CXX_TESTS} itkSupervisedImageClassifierTest)
add_test( itkThresholdMaximumConnectedComponentsImageFilterTest1
          ${CXX_TESTS}
  --compare ${BASELINE}/itkThresholdMaximumConnectedComponentsImageFilterTest1.png
            ${TEMP}/itkThresholdMaximumConnectedComponentsImageFilterTest1.png
          itkThresholdMaximumConnectedComponentsImageFilterTest
          ${INPUTDATA}/CellsFluorescence1.png
          ${TEMP}/itkThresholdMaximumConnectedComponentsImageFilterTest1.png 10)

add_test( itkThresholdMaximumConnectedComponentsImageFilterTest2
          ${CXX_TESTS}
  --compare ${BASELINE}/itkThresholdMaximumConnectedComponentsImageFilterTest2.png
            ${TEMP}/itkThresholdMaximumConnectedComponentsImageFilterTest2.png
          itkThresholdMaximumConnectedComponentsImageFilterTest
          ${INPUTDATA}/CellsFluorescence2.png
          ${TEMP}/itkThresholdMaximumConnectedComponentsImageFilterTest2.png 10)

add_test(itkThresholdSegmentationLevelSetImageFilterTest ${CXX_TESTS} itkThresholdSegmentationLevelSetImageFilterTest)
add_test(itkGibbsTest ${CXX_TESTS} itkGibbsTest)
add_test(itkDeformableTest ${CXX_TESTS} itkDeformableTest)
add_test(itkDeformableSimplexMesh3DBalloonForceFilterTest ${CXX_TESTS} itkDeformableSimplexMesh3DBalloonForceFilterTest)
add_test(itkDeformableSimplexMesh3DGradientConstaintForceFilterTest ${CXX_TESTS} itkDeformableSimplexMesh3DGradientConstraintForceFilterTest)
add_test(itkDeformableSimplexMesh3DFilterTest ${CXX_TESTS} itkDeformableSimplexMesh3DFilterTest)
add_test(itk2DDeformableTest ${CXX_TESTS} itk2DDeformableTest)
add_test(itkBinaryThinningImageFilterTest ${CXX_TESTS}
   --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/BinaryThinningImageFilterTest.png
             ${ITK_TEST_OUTPUT_DIR}/BinaryThinningImageFilterTest.png
  itkBinaryThinningImageFilterTest
             ${ITK_DATA_ROOT}/Input/Shapes.png
             ${ITK_TEST_OUTPUT_DIR}/BinaryThinningImageFilterTest.png
             )
add_test(itkSphereMeshSourceTest ${CXX_TESTS} itkSphereMeshSourceTest)
add_test(itkUnsharpMaskLevelSetImageFilterTest ${CXX_TESTS} itkUnsharpMaskLevelSetImageFilterTest)
add_test(itkVnlFFTTest ${CXX_TESTS} itkVnlFFTTest)
if(USE_FFTWF)
       add_test(itkFFTWF_FFTTest ${CXX_TESTS} itkFFTWF_FFTTest ${ITK_TEST_OUTPUT_DIR} )

       add_test(itkVnlFFTWF_FFTTest ${CXX_TESTS} itkVnlFFTWF_FFTTest )
       set_tests_properties(itkVnlFFTWF_FFTTest PROPERTIES ENVIRONMENT
         "ITK_FFTW_READ_WISDOM_CACHE=oN;ITK_FFTW_WRITE_WISDOM_CACHE=oN;ITK_FFTW_WISDOM_CACHE_FILE=${ITK_TEST_OUTPUT_DIR}/.wisdom_from_ITK_FFTW_WISDOM_CACHE_FILE;ITK_FFTW_PLAN_RIGOR=FFTW_EXHAUSTIVE"
         )

endif(USE_FFTWF)
if(USE_FFTWD)
       add_test(itkFFTWD_FFTTest ${CXX_TESTS} itkFFTWD_FFTTest ${ITK_TEST_OUTPUT_DIR} )

       add_test(itkVnlFFTWD_FFTTest ${CXX_TESTS} itkVnlFFTWD_FFTTest)
       set_tests_properties(itkVnlFFTWD_FFTTest PROPERTIES ENVIRONMENT
         "ITK_FFTW_READ_WISDOM_CACHE=oN;ITK_FFTW_WISDOM_CACHE_BASE=${ITK_TEST_OUTPUT_DIR};ITK_FFTW_PLAN_RIGOR=FFTW_EXHAUSTIVE;ITK_FFTW_WRITE_WISDOM_CACHE=oN")
endif(USE_FFTWD)
if(USE_FFTWD)
       add_test(itkCurvatureRegistrationFilterTest ${CXX_TESTS} itkCurvatureRegistrationFilterTest)
endif(USE_FFTWD)

add_test(itkMIRegistrationFunctionTest ${CXX_TESTS} itkMIRegistrationFunctionTest)
add_test(itkSymmetricForcesDemonsRegistrationFilterTest ${CXX_TESTS} itkSymmetricForcesDemonsRegistrationFilterTest)
add_test(itkVoronoiDiagram2DTest ${CXX_TESTS} itkVoronoiDiagram2DTest)
add_test(itkVoronoiSegmentationImageFilterTest ${CXX_TESTS} itkVoronoiSegmentationImageFilterTest)
add_test(itkVoronoiSegmentationRGBImageFilterTest ${CXX_TESTS} itkVoronoiSegmentationRGBImageFilterTest)
add_test(itkWatershedImageFilterTest ${CXX_TESTS} itkWatershedImageFilterTest)
add_test(itkPointSetToPointSetRegistrationTest ${CXX_TESTS}  itkPointSetToPointSetRegistrationTest)
add_test(itkPointSetToSpatialObjectDemonsRegistrationTest ${CXX_TESTS}  itkPointSetToSpatialObjectDemonsRegistrationTest)

add_test(itkCurvatureFlowTest ${CXX_TESTS}
    itkCurvatureFlowTest ${ITK_TEST_OUTPUT_DIR}/itkCurvatureFlowTest.vtk)

# Metric fails when centered pixel coordinates not in use
add_test(itkBinaryMedialNodeMetricTest ${CXX_TESTS} itkBinaryMedialNodeMetricTest)

add_test( itkQuadEdgeMeshQuadricDecimationTest ${CXX_TESTS}
  itkQuadEdgeMeshQuadricDecimationTest
  ${INPUTDATA}/mushroom.vtk
  100
  ${TEMP}/temp_QuadricDecimationResult1.vtk
  )
add_test( itkQuadEdgeMeshQuadricDecimationTetrahedronTest ${CXX_TESTS}
  itkQuadEdgeMeshQuadricDecimationTest
  ${INPUTDATA}/tetrahedron.vtk
  2
  ${TEMP}/temp_QuadricDecimationTetrahedron.vtk
  )
add_test( itkQuadEdgeMeshSquaredEdgeLengthDecimationTest ${CXX_TESTS}
  itkQuadEdgeMeshSquaredEdgeLengthDecimationTest
  ${INPUTDATA}/mushroom.vtk
  20
  ${TEMP}/temp_SquaredEdgeLengthDecimationResult1.vtk
  )
add_test( itkQuadEdgeMeshCleanFilterTest
  ${CXX_TESTS}
  itkQuadEdgeMeshCleanFilterTest
  ${INPUTDATA}/mushroom.vtk
  0.1
  ${TEMP}/temp_CleanResult1.vtk
  )
add_test( itkQuadEdgeMeshNormalFilterTest0 ${CXX_TESTS}
  itkQuadEdgeMeshNormalFilterTest
  ${INPUTDATA}/mushroom.vtk
  0
)
add_test( itkQuadEdgeMeshNormalFilterTest1 ${CXX_TESTS}
  itkQuadEdgeMeshNormalFilterTest
  ${INPUTDATA}/mushroom.vtk
  1
)
add_test( itkQuadEdgeMeshNormalFilterTest2 ${CXX_TESTS}
  itkQuadEdgeMeshNormalFilterTest
  ${INPUTDATA}/mushroom.vtk
  2
)

add_test( itkQuadEdgeMeshSmoothingTest0 ${CXX_TESTS}
  itkQuadEdgeMeshSmoothingTest
  ${INPUTDATA}/genusZeroSurface01.vtk
  10
  0.1
  0
  ${TEMP}/temp_SmoothResult0.vtk
  )

add_test( itkQuadEdgeMeshSmoothingTest1 ${CXX_TESTS}
  itkQuadEdgeMeshSmoothingTest
  ${INPUTDATA}/genusZeroSurface01.vtk
  10
  0.1
  1
  ${TEMP}/temp_SmoothResult1.vtk
  )

set( CURV_TESTS Gaussian Max Mean Min )
foreach( loop_var ${CURV_TESTS} )
  add_test( itkQuadEdgeMesh${loop_var}CurvatureTest
    ${CXX_TESTS}
    itkQuadEdgeMesh${loop_var}CurvatureTest
    ${INPUTDATA}/mushroom.vtk
    )
endforeach( loop_var )

foreach( border 0 1 )
  foreach( coeff 0 1 2 3 4 )
    add_test( itkQuadEdgeMeshLinearParameterizationTest${border}_${coeff}
    ${CXX_TESTS}
    itkQuadEdgeMeshLinearParameterizationTest
    ${INPUTDATA}/mushroom.vtk ${border} ${coeff}
    ${TEMP}/param_border${border}_coeff${coeff}.vtk
    )
  endforeach( coeff )
endforeach( border )

add_test( itkQuadEdgeMeshDelaunayConformingFilterTest ${CXX_TESTS}
  itkQuadEdgeMeshDelaunayConformingFilterTest
  ${INPUTDATA}/mushroom.vtk
  ${TEMP}/mushrom_delaunay.vtk
  )

add_executable(itkAlgorithmsPrintTest itkAlgorithmsPrintTest.cxx)
target_link_libraries(itkAlgorithmsPrintTest ITKAlgorithms ITKFEM ITKNumerics)
add_executable(itkAlgorithmsPrintTest2 itkAlgorithmsPrintTest2.cxx)
target_link_libraries(itkAlgorithmsPrintTest2 ITKAlgorithms ITKFEM ITKNumerics)

# Avoid bogus uninitialied warning in itkWatershedSegementTreeGenerator
# by Sun compilers
include(${ITK_SOURCE_DIR}/CMake/itkCheckCXXAcceptsFlags.cmake)
itkCHECK_CXX_ACCEPTS_FLAGS("-features=no%anachronisms" SUN_COMPILER)
if(SUN_COMPILER)
  set_source_files_properties(itkAlgorithmsPrintTest3.cxx
                        PROPERTIES COMPILE_FLAGS -w)
endif(SUN_COMPILER)

add_executable(itkAlgorithmsPrintTest3 itkAlgorithmsPrintTest3.cxx)
target_link_libraries(itkAlgorithmsPrintTest3 ITKAlgorithms ITKFEM ITKNumerics)

add_executable(itkAlgorithmsPrintTest4 itkAlgorithmsPrintTest4.cxx)
target_link_libraries(itkAlgorithmsPrintTest4 ITKAlgorithms ITKFEM
                                              ITKStatistics ITKNumerics)

if(ITK_OBJCXX_COMPILER_WORKS)
  set(ALGORITHMS_OBJCXX_TEST ${CXX_TEST_PATH}/itkAlgorithmsHeaderObjCxxTest)
  add_test(itkAlgorithmsHeaderObjCxxTest ${ALGORITHMS_OBJCXX_TEST})
  add_executable(itkAlgorithmsHeaderObjCxxTest itkAlgorithmsHeaderObjCxxTest.mm)
  target_link_libraries(itkAlgorithmsHeaderObjCxxTest ITKAlgorithms ITKFEM ITKNumerics)
endif(ITK_OBJCXX_COMPILER_WORKS)

add_executable(itkAlgorithmsHeaderTest itkAlgorithmsHeaderTest.cxx)
target_link_libraries(itkAlgorithmsHeaderTest ITKAlgorithms ITKFEM ITKNumerics)
add_test(itkAlgorithmsHeaderTest ${ALGORITHMS_HEADER_TESTS})

if(ITK_DATA_ROOT)

add_test(itkMatchCardinalityImageToImageMetricTest ${CXX_TESTS}
  itkMatchCardinalityImageToImageMetricTest ${ITK_DATA_ROOT}/Input/Spots.png)

add_test(itkKappaStatisticImageToImageMetricTest ${CXX_TESTS}
  itkKappaStatisticImageToImageMetricTest ${ITK_DATA_ROOT}/Input/Spots.png)

add_test(itkIsolatedWatershedImageFilterTest ${CXX_TESTS}
   --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/IsolatedWatershedImageFilterTest.png
             ${ITK_TEST_OUTPUT_DIR}/IsolatedWatershedImageFilterTest.png
  itkIsolatedWatershedImageFilterTest
             ${ITK_DATA_ROOT}/Input/cthead1.png
             ${ITK_TEST_OUTPUT_DIR}/IsolatedWatershedImageFilterTest.png
             113 84 120 99)

add_test(itkOtsuThresholdImageFilterTest ${CXX_TESTS}
  --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuThresholdImageFilterTest.png
            ${ITK_TEST_OUTPUT_DIR}/OtsuThresholdImageFilterTest.png
  itkOtsuThresholdImageFilterTest
            ${ITK_DATA_ROOT}/Input/cthead1.png
            ${ITK_TEST_OUTPUT_DIR}/OtsuThresholdImageFilterTest.png
            64)

add_test(itkOtsuMultipleThresholdsImageFilterTest ${CXX_TESTS}
  --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png
            ${ITK_TEST_OUTPUT_DIR}/OtsuMultipleThresholdsImageFilterTest.png
  itkOtsuMultipleThresholdsImageFilterTest
            ${ITK_DATA_ROOT}/Input/peppers.png
            ${ITK_TEST_OUTPUT_DIR}/OtsuMultipleThresholdsImageFilterTest.png
            64 3 2)

add_test(itkVoronoiPartitioningImageFilterTest1 ${CXX_TESTS}
  --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.png
            ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest1.png
  itkVoronoiPartitioningImageFilterTest
            ${ITK_DATA_ROOT}/Input/sf4.png
            ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest1.png 1)

add_test(itkVoronoiPartitioningImageFilterTest2 ${CXX_TESTS}
  --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/VoronoiPartioningImageFilterTest2.png
            ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest2.png
  itkVoronoiPartitioningImageFilterTest
            ${ITK_DATA_ROOT}/Input/sf4.png
            ${ITK_TEST_OUTPUT_DIR}/VoronoiPartioningImageFilterTest2.png 0)

add_test(itkVectorThresholdSegmentationLevelSetImageFilterTest ${CXX_TESTS}
  --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/VectorThresholdSegmentationLevelSetImageFilterTest.png
            ${ITK_TEST_OUTPUT_DIR}/VectorThresholdSegmentationLevelSetImageFilterTest.png
  itkVectorThresholdSegmentationLevelSetImageFilterTest
            ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSliceInitialLevelSet.png
            ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png
            ${ITK_TEST_OUTPUT_DIR}/VectorThresholdSegmentationLevelSetImageFilterTest.png
            5.0 50
            )

add_test(itkSTAPLEImageFilterTest ${CXX_TESTS}
   --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/STAPLEImageFilterTest.mha
             ${ITK_TEST_OUTPUT_DIR}/STAPLEImageFilterTest.mha
   itkSTAPLEImageFilterTest
             2 ${ITK_TEST_OUTPUT_DIR}/STAPLEImageFilterTest.mha 255 0.5
             ${ITK_DATA_ROOT}/Input/STAPLE1.png
             ${ITK_DATA_ROOT}/Input/STAPLE2.png
             ${ITK_DATA_ROOT}/Input/STAPLE3.png
             ${ITK_DATA_ROOT}/Input/STAPLE4.png
            )

endif(ITK_DATA_ROOT)

endif( NOT ITK_DISABLE_CXX_TESTING )


if( NOT ITK_DISABLE_TCL_TESTING )
if(ITK_CSWIG_TCL)

  add_test(itkCurvatureFlowTestTcl2 ${ITKWISH_EXECUTABLE}
           ${ITKTesting_SOURCE_DIR}/Code/Algorithms/itkCurvatureFlowTestTcl2.tcl)

  add_test(ImageRegistration3Tcl ${ITKWISH_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration3.tcl
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration3TclTest.png )

  add_test(ImageRegistration4Tcl ${ITKWISH_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration4.tcl
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration4TclTest.png )

  add_test(ImageRegistration5Tcl ${ITKWISH_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration5.tcl
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration5TclTest.png )

  add_test(VoronoiSegmentationTcl ${ITKWISH_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Segmentation/VoronoiSegmentation.tcl
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
           ${ITK_SOURCE_DIR}/Examples/Data/VentricleModel.png
           ${ITK_TEST_OUTPUT_DIR}/VoronoiSegmentationTcl.png  0.3 0.3 )

  add_test(WatershedSegmentation1Tcl ${ITKWISH_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Segmentation/WatershedSegmentation1.tcl
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
           ${ITK_TEST_OUTPUT_DIR}/WatershedSegmentation1Tcl.mhd )

endif(ITK_CSWIG_TCL)
endif( NOT ITK_DISABLE_TCL_TESTING )


if( NOT ITK_DISABLE_PYTHON_TESTING )
if(ITK_CSWIG_PYTHON)

  add_test(itkCurvatureFlowTestPython2 ${PYTHON_EXECUTABLE}
           ${ITKTesting_SOURCE_DIR}/Code/Algorithms/itkCurvatureFlowTestPython2.py)

  add_test(ImageRegistration3Python ${PYTHON_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration3.py
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration3PythonTest.png )

  add_test(ImageRegistration4Python ${PYTHON_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration4.py
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration4PythonTest.png )

  add_test(ImageRegistration5Python ${PYTHON_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Registration/ImageRegistration5.py
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
           ${ITK_TEST_OUTPUT_DIR}/ImageRegistration5PythonTest.png )

  add_test(VoronoiSegmentationPython ${PYTHON_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Segmentation/VoronoiSegmentation.py
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
           ${ITK_SOURCE_DIR}/Examples/Data/VentricleModel.png
           ${ITK_TEST_OUTPUT_DIR}/VoronoiSegmentationPython.png  0.3 0.3 )

  add_test(WatershedSegmentation1Python ${PYTHON_EXECUTABLE}
           ${ITK_SOURCE_DIR}/Examples/Segmentation/WatershedSegmentation1.py
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
           ${ITK_TEST_OUTPUT_DIR}/WatershedSegmentation1Python.mhd )

endif(ITK_CSWIG_PYTHON)
endif( NOT ITK_DISABLE_PYTHON_TESTING )


if( NOT ITK_DISABLE_JAVA_TESTING )
if(ITK_CSWIG_JAVA AND ITK_BINARY_DIR)
  set(JAVACLASSPATH ${ITK_BINARY_DIR}/Wrapping/CSwig/Java/InsightToolkit.jar)
  foreach(cfgtype ${CMAKE_CONFIGURATION_TYPES})
    set(JAVACLASSPATH "${JAVACLASSPATH}\\;${ITK_BINARY_DIR}/Wrapping/CSwig/Java/${cfgtype}/InsightToolkit.jar")
  endforeach(cfgtype)

  foreach( javatestfile
     VoronoiSegmentation
     WatershedSegmentation1
     )

  add_custom_target(${javatestfile}Java ALL  DEPENDS ${ITK_BINARY_DIR}/Examples/Segmentation/${javatestfile}.class)

  add_custom_command(
    OUTPUT  ${ITK_BINARY_DIR}/Examples/Segmentation/${javatestfile}.class
    DEPENDS ${ITK_SOURCE_DIR}/Examples/Segmentation/${javatestfile}.java
    COMMAND ${JAVA_COMPILE}
    ARGS -classpath "${JAVACLASSPATH}" -d "${ITK_BINARY_DIR}/Examples/Segmentation"
    ${ITK_SOURCE_DIR}/Examples/Segmentation/${javatestfile}.java
    COMMENT "Java Class")

  endforeach( javatestfile )

  if(WIN32)
    set(TMPJAVACLASSPATH ${ITK_BINARY_DIR}/Examples/Segmentation/)
    foreach(javadirectory ${JAVACLASSPATH})
      # Set path separator for Windows
      set(TMPJAVACLASSPATH "${TMPJAVACLASSPATH}\\;${javadirectory}")
    endforeach(javadirectory)
    set(JAVACLASSPATH ${TMPJAVACLASSPATH})
  else(WIN32)
    set(TMPJAVACLASSPATH ${ITK_BINARY_DIR}/Examples/Segmentation/)
    foreach(javadirectory ${JAVACLASSPATH})
      # Set path separator for UNIX
      set(TMPJAVACLASSPATH "${TMPJAVACLASSPATH}:${javadirectory}")
    endforeach(javadirectory)
    set(JAVACLASSPATH ${TMPJAVACLASSPATH})
  endif(WIN32)

  add_test(VoronoiSegmentationJava ${JAVA_RUNTIME} -classpath "${JAVACLASSPATH}"
           VoronoiSegmentation
           ${ITK_SOURCE_DIR}/Examples/Data/BrainT1Slice.png
           ${ITK_SOURCE_DIR}/Examples/Data/VentricleModel.png
           ${ITK_TEST_OUTPUT_DIR}/VoronoiSegmentationJava.png  0.3 0.3 )

  add_test(WatershedSegmentation1Java ${JAVA_RUNTIME} -classpath "${JAVACLASSPATH}"
           WatershedSegmentation1
           ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
           ${ITK_TEST_OUTPUT_DIR}/WatershedSegmentation1Java.mhd  )


endif(ITK_CSWIG_JAVA AND ITK_BINARY_DIR)
endif( NOT ITK_DISABLE_JAVA_TESTING )
