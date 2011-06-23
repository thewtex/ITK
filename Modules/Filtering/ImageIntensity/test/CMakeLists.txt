itk_module_test()
set(ITK_ImageIntensityTests
itkCosImageFilterAndAdaptorTest.cxx
itkExpImageFilterAndAdaptorTest.cxx
itkVectorRescaleIntensityImageFilterTest.cxx
itkTernaryMagnitudeSquaredImageFilterTest.cxx
itkMinimumImageFilterTest.cxx
itkLog10ImageFilterAndAdaptorTest.cxx
itkVectorIndexSelectionCastImageFilterTest.cxx
itkInvertIntensityImageFilterTest.cxx
itkSymmetricEigenAnalysisImageFilterTest.cxx
itkEdgePotentialImageFilterTest.cxx
itkComplexToModulusFilterAndAdaptorTest.cxx
itkAddImageAdaptorTest.cxx
itkAndImageFilterTest.cxx
itkAdaptImageFilterTest2.cxx
itkLogImageFilterAndAdaptorTest.cxx
itkNotImageFilterTest.cxx
itkImageIntensityHeaderTest.cxx
itkImageAdaptorNthElementTest.cxx
itkConstrainedValueAdditionImageFilterTest.cxx
itkAtanImageFilterAndAdaptorTest.cxx
itkMaskNegatedImageFilterTest.cxx
itkAddImageFilterTest.cxx
itkMultiplyImageFilterTest.cxx
itkWeightedAddImageFilterTest.cxx
itkRescaleIntensityImageFilterTest.cxx
itkNormalizeImageFilterTest.cxx
itkNaryAddImageFilterTest.cxx
itkShiftScaleImageFilterTest.cxx
itkComplexToPhaseFilterAndAdaptorTest.cxx
itkIntensityWindowingImageFilterTest.cxx
itkTernaryMagnitudeImageFilterTest.cxx
itkAbsImageFilterAndAdaptorTest.cxx
itkMaximumImageFilterTest.cxx
itkBinaryMagnitudeImageFilterTest.cxx
itkMatrixIndexSelectionImageFilterTest.cxx
itkSquareImageFilterTest.cxx
itkRGBToVectorAdaptImageFilterTest.cxx
itkComplexToRealFilterAndAdaptorTest.cxx
itkNaryMaximumImageFilterTest.cxx
itkAtan2ImageFilterTest.cxx
itkSqrtImageFilterAndAdaptorTest.cxx
itkAsinImageFilterAndAdaptorTest.cxx
itkMaskImageFilterTest.cxx
itkHistogramMatchingImageFilterTest.cxx
itkAcosImageFilterAndAdaptorTest.cxx
itkExpNegativeImageFilterAndAdaptorTest.cxx
itkTanImageFilterAndAdaptorTest.cxx
itkSigmoidImageFilterTest.cxx
itkDivideImageFilterTest.cxx
itkDivideImageFilterTest2.cxx
itkVectorExpandImageFilterTest.cxx
itkAdaptImageFilterTest.cxx
itkOrImageFilterTest.cxx
itkRGBToLuminanceImageFilterAndAdaptorTest.cxx
itkXorImageFilterTest.cxx
itkSubtractImageFilterTest.cxx
itkComplexToImaginaryFilterAndAdaptorTest.cxx
itkVectorToRGBImageAdaptorTest.cxx
itkSinImageFilterAndAdaptorTest.cxx
itkPolylineMask2DImageFilterTest.cxx
itkPolylineMaskImageFilterTest.cxx
itkPromoteDimensionImageTest.cxx
itkModulusImageFilterTest.cxx
itkVectorMagnitudeImageFilterTest.cxx
itkNormalizeToConstantImageFilterTest.cxx
)

CreateTestDriver(ITK_ImageIntensity  "${ITK_ImageIntensity-Test_LIBRARIES}" "${ITK_ImageIntensityTests}")

itk_add_test(NAME itkImageIntensityHeaderTest
      COMMAND ITK_ImageIntensityTestDriver itkImageIntensityHeaderTest)
itk_add_test(NAME itkCosImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkCosImageFilterAndAdaptorTest)
itk_add_test(NAME itkVectorRescaleIntensityImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkVectorRescaleIntensityImageFilterTest)
itk_add_test(NAME itkTernaryMagnitudeSquaredImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkTernaryMagnitudeSquaredImageFilterTest)
itk_add_test(NAME itkMinimumImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkMinimumImageFilterTest)
itk_add_test(NAME itkLog10ImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkLog10ImageFilterAndAdaptorTest)
itk_add_test(NAME itkVectorIndexSelectionCastImageFilterTest1
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest1.mha
              ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest1.mha
    itkVectorIndexSelectionCastImageFilterTest ${ITK_DATA_ROOT}/Input/RGBTestImage.tif ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest1.mha 0)
itk_add_test(NAME itkVectorIndexSelectionCastImageFilterTest2
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest2.mha
              ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest2.mha
    itkVectorIndexSelectionCastImageFilterTest ${ITK_DATA_ROOT}/Input/RGBTestImage.tif ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest2.mha 1)
itk_add_test(NAME itkVectorIndexSelectionCastImageFilterTest3
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest3.mha
              ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest3.mha
    itkVectorIndexSelectionCastImageFilterTest ${ITK_DATA_ROOT}/Input/RGBTestImage.tif ${ITK_TEST_OUTPUT_DIR}/itkVectorIndexSelectionCastImageFilterTest3.mha 2)
itk_add_test(NAME itkInvertIntensityImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/InvertIntensityImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/InvertIntensityImageFilterTest.png
    itkInvertIntensityImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/InvertIntensityImageFilterTest.png)
itk_add_test(NAME itkSymmetricEigenAnalysisImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkSymmetricEigenAnalysisImageFilterTest)
itk_add_test(NAME itkEdgePotentialImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkEdgePotentialImageFilterTest)
itk_add_test(NAME itkComplexToModulusFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkComplexToModulusFilterAndAdaptorTest)
itk_add_test(NAME itkAddImageAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkAddImageAdaptorTest)
itk_add_test(NAME itkAndImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkAndImageFilterTest)
itk_add_test(NAME itkAdaptImageFilterTest2
      COMMAND ITK_ImageIntensityTestDriver itkAdaptImageFilterTest2)
itk_add_test(NAME itkLogImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkLogImageFilterAndAdaptorTest)
itk_add_test(NAME itkNotImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkNotImageFilterTest)
itk_add_test(NAME itkImageAdaptorNthElementTest
      COMMAND ITK_ImageIntensityTestDriver itkImageAdaptorNthElementTest)
itk_add_test(NAME itkConstrainedValueAdditionImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkConstrainedValueAdditionImageFilterTest)
itk_add_test(NAME itkAtanImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkAtanImageFilterAndAdaptorTest)
itk_add_test(NAME itkMaskNegatedImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkMaskNegatedImageFilterTest)
itk_add_test(NAME itkAddImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkAddImageFilterTest)
itk_add_test(NAME itkMultiplyImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkMultiplyImageFilterTest)
itk_add_test(NAME itkWeightedAddImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkWeightedAddImageFilterTest)
itk_add_test(NAME itkRescaleIntensityImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkRescaleIntensityImageFilterTest)
itk_add_test(NAME itkNormalizeImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkNormalizeImageFilterTest)
itk_add_test(NAME itkNaryAddImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkNaryAddImageFilterTest)
itk_add_test(NAME itkShiftScaleImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkShiftScaleImageFilterTest)
itk_add_test(NAME itkComplexToPhaseFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkComplexToPhaseFilterAndAdaptorTest)
itk_add_test(NAME itkIntensityWindowingImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkIntensityWindowingImageFilterTest)
itk_add_test(NAME itkTernaryMagnitudeImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkTernaryMagnitudeImageFilterTest)
itk_add_test(NAME itkAbsImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkAbsImageFilterAndAdaptorTest)
itk_add_test(NAME itkMaximumImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkMaximumImageFilterTest)
itk_add_test(NAME itkBinaryMagnitudeImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkBinaryMagnitudeImageFilterTest)
itk_add_test(NAME itkMatrixIndexSelectionImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/MatrixIndexSelectionImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/MatrixIndexSelectionImageFilterTest.png
    itkMatrixIndexSelectionImageFilterTest ${ITK_TEST_OUTPUT_DIR}/MatrixIndexSelectionImageFilterTest.png)
itk_add_test(NAME itkSquareImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkSquareImageFilterTest)
itk_add_test(NAME itkRGBToVectorAdaptImageFilterTest.cxx
      COMMAND ITK_ImageIntensityTestDriver itkRGBToVectorAdaptImageFilterTest)
itk_add_test(NAME itkComplexToRealFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkComplexToRealFilterAndAdaptorTest)
itk_add_test(NAME itkNaryMaximumImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkNaryMaximumImageFilterTest)
itk_add_test(NAME itkAtan2ImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkAtan2ImageFilterTest)
itk_add_test(NAME itkSqrtImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkSqrtImageFilterAndAdaptorTest)
itk_add_test(NAME itkAsinImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkAsinImageFilterAndAdaptorTest)
itk_add_test(NAME itkMaskImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkMaskImageFilterTest)
itk_add_test(NAME itkHistogramMatchingImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkHistogramMatchingImageFilterTest)
itk_add_test(NAME itkAcosImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkAcosImageFilterAndAdaptorTest)
itk_add_test(NAME itkExpNegativeImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkExpNegativeImageFilterAndAdaptorTest)
itk_add_test(NAME itkExpImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkExpImageFilterAndAdaptorTest)
itk_add_test(NAME itkTanImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkTanImageFilterAndAdaptorTest)
itk_add_test(NAME itkSigmoidImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkSigmoidImageFilterTest)
itk_add_test(NAME itkDivideImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkDivideImageFilterTest)
itk_add_test(NAME itkDivideImageFilterTest2
      COMMAND ITK_ImageIntensityTestDriver itkDivideImageFilterTest2)
itk_add_test(NAME itkVectorExpandImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkVectorExpandImageFilterTest)
itk_add_test(NAME itkAdaptImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkAdaptImageFilterTest)
itk_add_test(NAME itkOrImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkOrImageFilterTest)
itk_add_test(NAME itkRGBToLuminanceImageFilterAndAdaptorTest.cxx
      COMMAND ITK_ImageIntensityTestDriver itkRGBToLuminanceImageFilterAndAdaptorTest)
itk_add_test(NAME itkXorImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkXorImageFilterTest)
itk_add_test(NAME itkSubtractImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkSubtractImageFilterTest)
itk_add_test(NAME itkComplexToImaginaryFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkComplexToImaginaryFilterAndAdaptorTest)
itk_add_test(NAME itkVectorToRGBImageAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkVectorToRGBImageAdaptorTest)
itk_add_test(NAME itkSinImageFilterAndAdaptorTest
      COMMAND ITK_ImageIntensityTestDriver itkSinImageFilterAndAdaptorTest)
itk_add_test(NAME itkPolylineMask2DImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkPolylineMask2DImageFilterTest
              ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkPolylineMask2DImageFilterTest.png)
itk_add_test(NAME itkPolylineMaskImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkPolylineMaskImageFilterTest)
itk_add_test(NAME itkPromoteDimensionImageTest
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/PromoteDimensionImageTest.mhd
              ${ITK_TEST_OUTPUT_DIR}/PromoteDimensionImageTest.mhd
    itkPromoteDimensionImageTest ${ITK_DATA_ROOT}/Input/VisibleWomanEyeSlice.png ${ITK_TEST_OUTPUT_DIR}/PromoteDimensionImageTest.mhd)
itk_add_test(NAME itkModulusImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ModulusImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/ModulusImageFilterTest.png
    itkModulusImageFilterTest ${ITK_DATA_ROOT}/Input/Spots.png ${ITK_TEST_OUTPUT_DIR}/ModulusImageFilterTest.png)
itk_add_test(NAME itkVectorMagnitudeImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkVectorMagnitudeImageFilterTest)
itk_add_test(NAME itkNormalizeToConstantImageFilterTest
      COMMAND ITK_ImageIntensityTestDriver itkNormalizeToConstantImageFilterTest)
