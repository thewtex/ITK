itk_module_test()
set(ITK-IntegratedTestTests
itkFilterImageAddTest.cxx
itkNumericsPrintTest.cxx
itkPushPopTileImageFilterTest.cxx
itkRandomVariateGeneratorBaseTest.cxx
itkReleaseDataFilterTest.cxx
itkRemoveBoundaryObjectsTest.cxx
itkRemoveBoundaryObjectsTest2.cxx
itkRGBInterpolateImageFunctionTest.cxx
itkScalarImageKmeansImageFilter3DTest.cxx
itkScalarImageToCooccurrenceListSampleFilterTest.cxx
itkScalarImageToCooccurrenceMatrixFilterTest.cxx
itkScalarImageToCooccurrenceMatrixFilterTest2.cxx
itkScalarImageToTextureFeaturesFilterTest.cxx
itkShrinkImagePreserveObjectPhysicalLocations.cxx
itkShrinkImageStreamingTest.cxx
itkShrinkImageTest.cxx
itkSimpleFilterWatcherTest.cxx
itkSparseFrequencyContainer2Test.cxx
itkSparseImageTest.cxx
itkSpatialObjectToImageRegistrationTest.cxx
itkStandardDeviationPerComponentSampleFilterTest.cxx
itkStatisticsAlgorithmTest.cxx
itkStatisticsAlgorithmTest2.cxx
itkStatisticsTypesTest.cxx
itkStatisticsPrintTest.cxx
itkSTLThreadTest.cxx
itkStreamingImageFilterTest.cxx
itkStreamingImageFilterTest2.cxx
itkStreamingImageFilterTest3.cxx
itkSubsampleTest.cxx
itkSubsampleTest2.cxx
itkSubsampleTest3.cxx
itkSymmetricSecondRankTensorImageReadTest.cxx
itkSymmetricSecondRankTensorImageWriteReadTest.cxx
itkTDistributionTest.cxx
itkThreadDefsTest.cxx
itkTimeProbesTest.cxx
itkTopHatImageFilterTest.cxx
itkVectorGeometryTest.cxx
itkVectorImageReadWriteTest.cxx
itkVectorImageTest.cxx
itkVNLRoundProfileTest1.cxx
itkZeroFluxBoundaryConditionTest.cxx
QPropXORTest1.cxx
RBFTest1.cxx
XORTest1.cxx
XORTest2.cxx
itkCommonPrintTest.cxx
itkIOPrintTest.cxx
itkBasicFiltersPrintTest.cxx
itkNeuralNetworksPrintTest.cxx
itkSpatialObjectPrintTest.cxx
)

CreateTestDriver_SupportBuildInIOFactories(ITK-IntegratedTest  "${ITK-IntegratedTest-Test_LIBRARIES}" "${ITK-IntegratedTestTests}")

# temporary solution for all print tests. Each module should have its own print test later.
add_executable(itkAlgorithmsPrintTest  itkAlgorithmsPrintTest.cxx)
itk_module_target_label(itkAlgorithmsPrintTest)
target_link_libraries(itkAlgorithmsPrintTest ${ITK-IntegratedTest-Test_LIBRARIES})
itk_add_test(NAME itkAlgorithmsPrintTest COMMAND itkAlgorithmsPrintTest)

add_executable(itkAlgorithmsPrintTest2  itkAlgorithmsPrintTest2.cxx)
itk_module_target_label(itkAlgorithmsPrintTest2)
target_link_libraries(itkAlgorithmsPrintTest2 ${ITK-IntegratedTest-Test_LIBRARIES})
itk_add_test(NAME itkAlgorithmsPrintTest2 COMMAND itkAlgorithmsPrintTest)

add_executable(itkAlgorithmsPrintTest3  itkAlgorithmsPrintTest3.cxx)
itk_module_target_label(itkAlgorithmsPrintTest3)
target_link_libraries(itkAlgorithmsPrintTest3 ${ITK-IntegratedTest-Test_LIBRARIES})
itk_add_test(NAME itkAlgorithmsPrintTest3 COMMAND itkAlgorithmsPrintTest)

add_executable(itkAlgorithmsPrintTest4  itkAlgorithmsPrintTest4.cxx)
itk_module_target_label(itkAlgorithmsPrintTest4)
target_link_libraries(itkAlgorithmsPrintTest4 ${ITK-IntegratedTest-Test_LIBRARIES})
itk_add_test(NAME itkAlgorithmsPrintTest4 COMMAND itkAlgorithmsPrintTest)

#-------------------------------------------------------------------------
itk_add_test(NAME itkStatisticsPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkStatisticsPrintTest)
itk_add_test(NAME itkSpatialObjectPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkSpatialObjectPrintTest)
itk_add_test(NAME itkIOPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkIOPrintTest)
itk_add_test(NAME itkCommonPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkCommonPrintTest)
itk_add_test(NAME itkBasicFiltersPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkBasicFiltersPrintTest)
itk_add_test(NAME itkNeuralNetworksPrintTest
      COMMAND ITK-IntegratedTestTestDriver itkNeuralNetworksPrintTest)
itk_add_test(NAME itkNumericsPrintTest
     COMMAND ITK-IntegratedTestTestDriver itkNumericsPrintTest)
itk_add_test(NAME itkPushPopTileImageFilterTest
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/PushPopTileImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/PushPopTileImageFilterTest.png
    itkPushPopTileImageFilterTest ${ITK_DATA_ROOT}/Input/Shapes.png ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/PushPopTileImageFilterTest.png)
itk_add_test(NAME itkRandomVariateGeneratorBaseTest
      COMMAND ITK-IntegratedTestTestDriver itkRandomVariateGeneratorBaseTest)
itk_add_test(NAME itkRemoveBoundaryObjectsTest
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/RemoveBoundaryObjectsTest.png
              ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest.png
    itkRemoveBoundaryObjectsTest ${ITK_DATA_ROOT}/Input/Spots.png ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest.png)
itk_add_test(NAME itkRemoveBoundaryObjectsTest2
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/RemoveBoundaryObjectsTest2.png
              ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest2.png
    itkRemoveBoundaryObjectsTest2 ${ITK_DATA_ROOT}/Input/SpotsInverted.png ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest2.png)
itk_add_test(NAME itkRGBInterpolateImageFunctionTest
      COMMAND ITK-IntegratedTestTestDriver itkRGBInterpolateImageFunctionTest)

itk_add_test(NAME itkScalarImageKmeansImageFilter3DTest
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
              ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
    itkScalarImageKmeansImageFilter3DTest ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1UCharRaw.nii.gz ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1RawSkullStrip.nii.gz ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz)
itk_add_test(NAME itkScalarImageToCooccurrenceListSampleFilterTest
      COMMAND ITK-IntegratedTestTestDriver itkScalarImageToCooccurrenceListSampleFilterTest)
itk_add_test(NAME itkScalarImageToCooccurrenceMatrixFilterTest
      COMMAND ITK-IntegratedTestTestDriver itkScalarImageToCooccurrenceMatrixFilterTest)
itk_add_test(NAME itkScalarImageToCooccurrenceMatrixFilterTest2
      COMMAND ITK-IntegratedTestTestDriver itkScalarImageToCooccurrenceMatrixFilterTest2)
itk_add_test(NAME itkScalarImageToTextureFeaturesFilterTest
      COMMAND ITK-IntegratedTestTestDriver itkScalarImageToTextureFeaturesFilterTest)
itk_add_test(NAME itkShrinkImagePreserveObjectPhysicalLocations
      COMMAND ITK-IntegratedTestTestDriver itkShrinkImagePreserveObjectPhysicalLocations)
itk_add_test(NAME itkShrinkImageStreamingTest
      COMMAND ITK-IntegratedTestTestDriver itkShrinkImageStreamingTest)
itk_add_test(NAME itkShrinkImageTest
      COMMAND ITK-IntegratedTestTestDriver itkShrinkImageTest)
itk_add_test(NAME itkSimpleFilterWatcherTest
      COMMAND ITK-IntegratedTestTestDriver itkSimpleFilterWatcherTest)
itk_add_test(NAME itkSparseFrequencyContainer2Test
      COMMAND ITK-IntegratedTestTestDriver itkSparseFrequencyContainer2Test)
itk_add_test(NAME itkSparseImageTest
      COMMAND ITK-IntegratedTestTestDriver itkSparseImageTest)
itk_add_test(NAME itkSpatialObjectToImageRegistrationTest
      COMMAND ITK-IntegratedTestTestDriver itkSpatialObjectToImageRegistrationTest)
itk_add_test(NAME itkStandardDeviationPerComponentSampleFilterTest
      COMMAND ITK-IntegratedTestTestDriver itkStandardDeviationPerComponentSampleFilterTest)
itk_add_test(NAME itkStatisticsAlgorithmTest
      COMMAND ITK-IntegratedTestTestDriver itkStatisticsAlgorithmTest)
itk_add_test(NAME itkStatisticsAlgorithmTest2
      COMMAND ITK-IntegratedTestTestDriver itkStatisticsAlgorithmTest2)
itk_add_test(NAME itkStatisticsTypesTest
      COMMAND ITK-IntegratedTestTestDriver itkStatisticsTypesTest)
itk_add_test(NAME itkSTLThreadTest
      COMMAND ITK-IntegratedTestTestDriver itkSTLThreadTest)
itk_add_test(NAME itkStreamingImageFilterTest
      COMMAND ITK-IntegratedTestTestDriver itkStreamingImageFilterTest)
itk_add_test(NAME itkStreamingImageFilterTest2
      COMMAND ITK-IntegratedTestTestDriver itkStreamingImageFilterTest2)
itk_add_test(NAME itkStreamingImageFilterTest3_1
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Input/CellsFluorescence1.png
              ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageFilterTest3_1.png
    itkStreamingImageFilterTest3 ${ITK_DATA_ROOT}/Input/CellsFluorescence1.png ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageFilterTest3_1.png 10)
itk_add_test(NAME itkStreamingImageFilterTest3_2
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Input/CellsFluorescence1.png
              ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageFilterTest3_2.png
    itkStreamingImageFilterTest3 ${ITK_DATA_ROOT}/Input/CellsFluorescence1.png ${ITK_TEST_OUTPUT_DIR}/itkStreamingImageFilterTest3_2.png 1000)
itk_add_test(NAME itkSubsampleTest
      COMMAND ITK-IntegratedTestTestDriver itkSubsampleTest)
itk_add_test(NAME itkSubsampleTest2
      COMMAND ITK-IntegratedTestTestDriver itkSubsampleTest2)
itk_add_test(NAME itkSubsampleTest3
      COMMAND ITK-IntegratedTestTestDriver itkSubsampleTest3)

itk_add_test(NAME itkSymmetricSecondRankTensorImageReadTest
      COMMAND ITK-IntegratedTestTestDriver itkSymmetricSecondRankTensorImageReadTest
              ${ITK_TEST_OUTPUT_DIR}/testSymmetricTensor.nrrd)
itk_add_test(NAME itkSymmetricSecondRankTensorImageWriteReadTest
      COMMAND ITK-IntegratedTestTestDriver itkSymmetricSecondRankTensorImageWriteReadTest
              ${ITK_TEST_OUTPUT_DIR}/testSymmetricTensorWriteRead.mha)
itk_add_test(NAME itkTDistributionTest
      COMMAND ITK-IntegratedTestTestDriver itkTDistributionTest)
itk_add_test(NAME itkThreadDefsTest
      COMMAND ITK-IntegratedTestTestDriver itkThreadDefsTest)
itk_add_test(NAME itkTimeProbesTest
      COMMAND ITK-IntegratedTestTestDriver itkTimeProbesTest)
itk_add_test(NAME itkBlackTopHatImageFilterTest
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkBlackTopHatImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkBlackTopHatImageFilterTest.png
    itkTopHatImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkBlackTopHatImageFilterTest.png 0 3)
itk_add_test(NAME itkWhiteTopHatImageFilterTest
      COMMAND ITK-IntegratedTestTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkWhiteTopHatImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkWhiteTopHatImageFilterTest.png
    itkTopHatImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkWhiteTopHatImageFilterTest.png 1 3)


add_executable(itkUnicodeIOTest itkUnicodeIOTest.cxx)
itk_module_target_label(itkUnicodeIOTest)
itk_add_test(NAME itkUnicodeIOTest COMMAND itkUnicodeIOTest)
itk_add_test(NAME itkVectorGeometryTest
      COMMAND ITK-IntegratedTestTestDriver itkVectorGeometryTest)
itk_add_test(NAME itkVectorImageReadWriteTest
      COMMAND ITK-IntegratedTestTestDriver itkVectorImageReadWriteTest
              ${ITK_TEST_OUTPUT_DIR}/VectorImageReadWriteTest.mhd)
itk_add_test(NAME itkVectorImageReadWriteTest2
      COMMAND ITK-IntegratedTestTestDriver itkVectorImageReadWriteTest
              ${ITK_TEST_OUTPUT_DIR}/VectorImageReadWriteTest.nrrd)
itk_add_test(NAME itkVectorImageTest
      COMMAND ITK-IntegratedTestTestDriver itkVectorImageTest
              ${ITK_TEST_OUTPUT_DIR}/VectorImage.nrrd ${ITK_TEST_OUTPUT_DIR}/VectorImage.mhd)
itk_add_test(NAME itkVNLRoundProfileTest1
      COMMAND ITK-IntegratedTestTestDriver itkVNLRoundProfileTest1)
itk_add_test(NAME itkZeroFluxBoundaryConditionTest
      COMMAND ITK-IntegratedTestTestDriver itkZeroFluxBoundaryConditionTest)
itk_add_test(NAME QPropXORTest1
      COMMAND ITK-IntegratedTestTestDriver QPropXORTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/qpropxortest.txt)
itk_add_test(NAME RBFTest1
      COMMAND ITK-IntegratedTestTestDriver RBFTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_test.txt)
itk_add_test(NAME XORTest1
      COMMAND ITK-IntegratedTestTestDriver XORTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
itk_add_test(NAME XORTest2
      COMMAND ITK-IntegratedTestTestDriver XORTest2
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
