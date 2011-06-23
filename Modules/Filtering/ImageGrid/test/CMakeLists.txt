itk_module_test()
set(ITK_ImageGridTests
itkBasicArchitectureTest.cxx
itkChangeInformationImageFilterTest.cxx
itkConstantPadImageTest.cxx
itkCyclicReferences.cxx
itkInterpolateImagePointsFilterTest.cxx
itkCropImageFilterTest.cxx
itkCropImageFilter3DTest.cxx
itkFlipImageFilterTest.cxx
itkExpandImageFilterTest.cxx
itkRegionOfInterestImageFilterTest.cxx
itkBSplineUpsampleImageFilterTest.cxx
itkBSplineResampleImageFilterTest.cxx
itkBSplineResampleImageFunctionTest.cxx
itkBSplineInterpolateImageFunctionTest.cxx
itkImageGridHeaderTest.cxx
itkWarpImageFilterTest2.cxx
itkBSplineDecompositionImageFilterTest.cxx
itkBSplineDownsampleImageFilterTest.cxx
itkTileImageFilterTest.cxx
itkInterpolateImageFilterTest.cxx
itkPasteImageFilterTest.cxx
itkVectorResampleImageFilterTest.cxx
itkPermuteAxesImageFilterTest.cxx
itkOrientedImage2DTest.cxx
itkOrientedImage3DTest.cxx
itkOrientedImageAdaptorTest.cxx
itkOrientedImageProfileTest1.cxx
itkOrientedImageProfileTest2.cxx
itkOrientedImageProfileTest3.cxx
itkOrientImageFilterTest.cxx
itkOrientImageFilterTest2.cxx
itkWarpImageFilterTest.cxx
itkWarpVectorImageFilterTest.cxx
itkWrapPadImageTest.cxx
itkMirrorPadImageTest.cxx
itkResampleImageTest.cxx
itkResampleImageTest2.cxx
itkResamplePhasedArray3DSpecialCoordinatesImageTest.cxx
itkPushPopTileImageFilterTest.cxx
itkShrinkImagePreserveObjectPhysicalLocations.cxx
itkShrinkImageStreamingTest.cxx
itkShrinkImageTest.cxx
itkZeroFluxNeumannPadImageFilterTest.cxx
)

CreateTestDriver(ITK_ImageGrid  "${ITK_ImageGrid-Test_LIBRARIES}" "${ITK_ImageGridTests}")

itk_add_test(NAME itkImageGridHeaderTest
      COMMAND ITK_ImageGridTestDriver itkImageGridHeaderTest)
itk_add_test(NAME itkBasicArchitectureTest
      COMMAND ITK_ImageGridTestDriver itkBasicArchitectureTest)
itk_add_test(NAME itkChangeInformationImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkChangeInformationImageFilterTest)
itk_add_test(NAME itkConstantPadImageTest
      COMMAND ITK_ImageGridTestDriver itkConstantPadImageTest)
itk_add_test(NAME itkCyclicReferences
      COMMAND ITK_ImageGridTestDriver itkCyclicReferences)
itk_add_test(NAME itkInterpolateImagePointsFilterTest
      COMMAND ITK_ImageGridTestDriver itkInterpolateImagePointsFilterTest)
itk_add_test(NAME itkCropImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkCropImageFilterTest)
itk_add_test(NAME itkCropImageFilter3DTest
      COMMAND ITK_ImageGridTestDriver itkCropImageFilter3DTest)
itk_add_test(NAME itkFlipImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkFlipImageFilterTest)
itk_add_test(NAME itkExpandImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkExpandImageFilterTest)
itk_add_test(NAME itkRegionOfInterestImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkRegionOfInterestImageFilterTest)
itk_add_test(NAME itkBSplineUpsampleImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkBSplineUpsampleImageFilterTest
              ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/itkBSplineUpsampleImageFilterTest1.mha 3)
itk_add_test(NAME itkBSplineResampleImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkBSplineResampleImageFilterTest)
itk_add_test(NAME itkBSplineResampleImageFunctionTest
      COMMAND ITK_ImageGridTestDriver itkBSplineResampleImageFunctionTest)
itk_add_test(NAME itkBSplineInterpolateImageFunctionTest
      COMMAND ITK_ImageGridTestDriver itkBSplineInterpolateImageFunctionTest)
itk_add_test(NAME itkWarpImageFilterTest2
      COMMAND ITK_ImageGridTestDriver itkWarpImageFilterTest2)
itk_add_test(NAME itkBSplineDecompositionImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkBSplineDecompositionImageFilterTest)
itk_add_test(NAME itkBSplineDownsampleImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkBSplineDownsampleImageFilterTest
              ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/itkBSplineDownsampleImageFilterTest1.mha 3)
itk_add_test(NAME itkTileImageFilterTest
      COMMAND ITK_ImageGridTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/TileImageFilterTest4.png
              ${ITK_TEST_OUTPUT_DIR}/TileImageFilterTest4.png
    itkTileImageFilterTest 1 2 0 ${ITK_DATA_ROOT}/Input/STAPLE1.png ${ITK_DATA_ROOT}/Input/STAPLE2.png ${ITK_DATA_ROOT}/Input/STAPLE3.png ${ITK_DATA_ROOT}/Input/STAPLE4.png ${ITK_DATA_ROOT}/Input/Shapes.png ${ITK_DATA_ROOT}/Input/Spots.png ${ITK_DATA_ROOT}/Input/SpotsInverted.png ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSliceInitialLevelSet.png ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_DATA_ROOT}/Input/cake_hard.png ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/sf4.png ${ITK_DATA_ROOT}/Input/smooth_circle.png ${ITK_DATA_ROOT}/Input/smooth_square.png ${ITK_TEST_OUTPUT_DIR}/TileImageFilterTest%d.png)
itk_add_test(NAME itkInterpolateImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkInterpolateImageFilterTest)
itk_add_test(NAME itkPasteImageFilterTest
      COMMAND ITK_ImageGridTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/PasteImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/PasteImageFilterTest.png
    itkPasteImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_TEST_OUTPUT_DIR}/PasteImageFilterTest.png)
itk_add_test(NAME itkVectorResampleImageFilterTest
      COMMAND ITK_ImageGridTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/VectorResampleImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/VectorResampleImageFilterTest.png
    itkVectorResampleImageFilterTest ${ITK_TEST_OUTPUT_DIR}/VectorResampleImageFilterTest.png)
itk_add_test(NAME itkPermuteAxesImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkPermuteAxesImageFilterTest)
itk_add_test(NAME itkOrientedImage2DTest01
      COMMAND ITK_ImageGridTestDriver itkOrientedImage2DTest
              ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySliceBorder20DirectionPlus30.mhd 0.000 0.000 191.392 110.500 -128.500 222.569 4.000 -8.000 7.4641 -4.9282)
itk_add_test(NAME itkOrientedImage3DTest01
      COMMAND ITK_ImageGridTestDriver itkOrientedImage3DTest
              ${ITK_DATA_ROOT}/Input/HeadMRVolumeWithDirection.mhd 0.000 0.000 0.000 166.277 -96.000 0.000 124.000 214.774 0.000 0.000 0.000 168.000 0.125 -4.125 9.000 -1.95425 -3.63485 9.000)
itk_add_test(NAME itkOrientedImageAdaptorTest
      COMMAND ITK_ImageGridTestDriver itkOrientedImageAdaptorTest)
itk_add_test(NAME itkOrientedImageProfileTest1
      COMMAND ITK_ImageGridTestDriver itkOrientedImageProfileTest1)
itk_add_test(NAME itkOrientedImageProfileTest2
      COMMAND ITK_ImageGridTestDriver itkOrientedImageProfileTest2)
itk_add_test(NAME itkOrientedImageProfileTest3
      COMMAND ITK_ImageGridTestDriver itkOrientedImageProfileTest3)
itk_add_test(NAME itkOrientImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkOrientImageFilterTest)
itk_add_test(NAME itkOrientImageFilterTest2
      COMMAND ITK_ImageGridTestDriver itkOrientImageFilterTest2)
itk_add_test(NAME itkWarpImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkWarpImageFilterTest)
itk_add_test(NAME itkWarpVectorImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkWarpVectorImageFilterTest)
itk_add_test(NAME itkWrapPadImageTest
      COMMAND ITK_ImageGridTestDriver itkWrapPadImageTest)
itk_add_test(NAME itkMirrorPadImageTest
      COMMAND ITK_ImageGridTestDriver itkMirrorPadImageTest)
itk_add_test(NAME itkResampleImageTest
      COMMAND ITK_ImageGridTestDriver itkResampleImageTest)
itk_add_test(NAME itkResampleImageTest2
      COMMAND ITK_ImageGridTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ResampleImageTest2.png
              ${ITK_TEST_OUTPUT_DIR}/ResampleImageTest2a.png
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ResampleImageTest2.png
              ${ITK_TEST_OUTPUT_DIR}/ResampleImageTest2b.png
    itkResampleImageTest2 ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_DATA_ROOT}/Input/circle.png ${ITK_TEST_OUTPUT_DIR}/ResampleImageTest2a.png ${ITK_TEST_OUTPUT_DIR}/ResampleImageTest2b.png)
itk_add_test(NAME itkResamplePhasedArray3DSpecialCoordinatesImageTest
      COMMAND ITK_ImageGridTestDriver itkResamplePhasedArray3DSpecialCoordinatesImageTest)
itk_add_test(NAME itkPushPopTileImageFilterTest
      COMMAND ITK_ImageGridTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/PushPopTileImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/PushPopTileImageFilterTest.png
    itkPushPopTileImageFilterTest ${ITK_DATA_ROOT}/Input/Shapes.png ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/PushPopTileImageFilterTest.png)
itk_add_test(NAME itkShrinkImagePreserveObjectPhysicalLocations
      COMMAND ITK_ImageGridTestDriver itkShrinkImagePreserveObjectPhysicalLocations)
itk_add_test(NAME itkShrinkImageStreamingTest
      COMMAND ITK_ImageGridTestDriver itkShrinkImageStreamingTest)
itk_add_test(NAME itkShrinkImageTest
      COMMAND ITK_ImageGridTestDriver itkShrinkImageTest)
itk_add_test(NAME itkZeroFluxNeumannPadImageFilterTest
      COMMAND ITK_ImageGridTestDriver itkZeroFluxNeumannPadImageFilterTest)
