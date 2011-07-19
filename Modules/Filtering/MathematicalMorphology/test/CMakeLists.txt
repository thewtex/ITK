itk_module_test()
set(ITKMathematicalMorphologyTests
itkClosingByReconstructionImageFilterTest.cxx
itkFlatStructuringElementTest.cxx
itkGrayscaleConnectedClosingImageFilterTest.cxx
itkGrayscaleConnectedOpeningImageFilterTest.cxx
itkGrayscaleFillholeImageFilterTest.cxx
itkGrayscaleFunctionDilateImageFilterTest.cxx
itkGrayscaleFunctionErodeImageFilterTest.cxx
itkGrayscaleMorphologicalClosingImageFilterTest.cxx
itkGrayscaleMorphologicalOpeningImageFilterTest.cxx
itkGrayscaleGeodesicErodeDilateImageFilterTest.cxx
itkHConvexConcaveImageFilterTest.cxx
itkMathematicalMorphologyHeaderTest.cxx
itkHMaximaMinimaImageFilterTest.cxx
itkMorphologicalGradientImageFilterTest.cxx
itkOpeningByReconstructionImageFilterTest.cxx
itkDoubleThresholdImageFilterTest.cxx
itkRemoveBoundaryObjectsTest.cxx
itkRemoveBoundaryObjectsTest2.cxx
itkShapedIteratorFromStructuringElementTest.cxx
itkTopHatImageFilterTest.cxx
)

CreateTestDriver(ITKMathematicalMorphology  "${ITKMathematicalMorphology-Test_LIBRARIES}" "${ITKMathematicalMorphologyTests}")

itk_add_test(NAME itkMathematicalMorphologyHeaderTest
      COMMAND ITKMathematicalMorphologyTestDriver itkMathematicalMorphologyHeaderTest)
itk_add_test(NAME itkClosingByReconstructionImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ClosingByReconstructionImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTest.png
    itkClosingByReconstructionImageFilterTest ${ITK_DATA_ROOT}/Input/closerec1.jpg ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTest.png 4 0 ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTestSubtract.png)
itk_add_test(NAME itkClosingByReconstructionImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/ClosingByReconstructionImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTest2.png
    itkClosingByReconstructionImageFilterTest ${ITK_DATA_ROOT}/Input/closerec1.jpg ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTest2.png 4 1 ${ITK_TEST_OUTPUT_DIR}/ClosingByReconstructionImageFilterTestSubtract2.png)
itk_add_test(NAME itkFlatStructuringElementTest
      COMMAND ITKMathematicalMorphologyTestDriver itkFlatStructuringElementTest)
itk_add_test(NAME itkGrayscaleConnectedClosingImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleConnectedClosingImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedClosingImageFilterTest.png
    itkGrayscaleConnectedClosingImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedClosingImageFilterTest.png 174 214)
itk_add_test(NAME itkGrayscaleConnectedClosingImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleConnectedClosingImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedClosingImageFilterTest2.png
    itkGrayscaleConnectedClosingImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedClosingImageFilterTest2.png 150 169)
itk_add_test(NAME itkGrayscaleConnectedOpeningImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleConnectedOpeningImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedOpeningImageFilterTest.png
    itkGrayscaleConnectedOpeningImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedOpeningImageFilterTest.png 174 214)
itk_add_test(NAME itkGrayscaleConnectedOpeningImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleConnectedOpeningImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedOpeningImageFilterTest2.png
    itkGrayscaleConnectedOpeningImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleConnectedOpeningImageFilterTest2.png 150 169)
itk_add_test(NAME itkGrayscaleFillholeImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleFillholeImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleFillholeImageFilterTest.png
    itkGrayscaleFillholeImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleFillholeImageFilterTest.png)
itk_add_test(NAME itkGrayscaleFunctionDilateImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkGrayscaleFunctionDilateImageFilterTest.mha
              ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleFunctionDilateImageFilterTest.mha
    itkGrayscaleFunctionDilateImageFilterTest ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleFunctionDilateImageFilterTest.mha)
itk_add_test(NAME itkGrayscaleFunctionErodeImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkGrayscaleFunctionErodeImageFilterTest.mha
              ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleFunctionErodeImageFilterTest.mha
    itkGrayscaleFunctionErodeImageFilterTest ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleFunctionErodeImageFilterTest.mha)
itk_add_test(NAME itkGrayscaleMorphologicalClosingImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleMorphologicalClosingImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleMorphologicalClosingImageFilterTest.png
    itkGrayscaleMorphologicalClosingImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleMorphologicalClosingImageFilterTest.png)
itk_add_test(NAME itkGrayscaleMorphologicalOpeningImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/GrayscaleMorphologicalOpeningImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/GrayscaleMorphologicalOpeningImageFilterTest.png
    itkGrayscaleMorphologicalOpeningImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/GrayscaleMorphologicalOpeningImageFilterTest.png)
itk_add_test(NAME itkGrayscaleGeodesicErodeDilateImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/HMaximaMinimaImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleGeodesicErodeDilateImageFilterTest.png
    itkGrayscaleGeodesicErodeDilateImageFilterTest ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_TEST_OUTPUT_DIR}/itkGrayscaleGeodesicErodeDilateImageFilterTest.png 35)
itk_add_test(NAME itkHConvexConcaveImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/HConvexConcaveImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/HConvexConcaveImageFilterTest.png
    itkHConvexConcaveImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/HConvexConcaveImageFilterTest.png 50)
itk_add_test(NAME itkHMaximaMinimaImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/HMaximaMinimaImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/HMaximaMinimaImageFilterTest.png
    itkHMaximaMinimaImageFilterTest ${ITK_DATA_ROOT}/Input/cake_easy.png ${ITK_TEST_OUTPUT_DIR}/HMaximaMinimaImageFilterTest.png 35)
itk_add_test(NAME itkHMaximaMinimaImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/HMaximaMinimaImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/HMaximaMinimaImageFilterTest2.png
    itkHMaximaMinimaImageFilterTest ${ITK_DATA_ROOT}/Input/cake_hard.png ${ITK_TEST_OUTPUT_DIR}/HMaximaMinimaImageFilterTest2.png 35)
itk_add_test(NAME itkMorphologicalGradientImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/MorphologicalGradientImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/MorphologicalGradientImageFilterTest.png
    itkMorphologicalGradientImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/MorphologicalGradientImageFilterTest.png)
itk_add_test(NAME itkOpeningByReconstructionImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/OpeningByReconstructionImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTest.png
    itkOpeningByReconstructionImageFilterTest ${ITK_DATA_ROOT}/Input/chondt.png ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTest.png 4 0 ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTestSubtract.png)
itk_add_test(NAME itkOpeningByReconstructionImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/OpeningByReconstructionImageFilterTest2.png
              ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTest2.png
    itkOpeningByReconstructionImageFilterTest ${ITK_DATA_ROOT}/Input/chondt.png ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTest2.png 4 1 ${ITK_TEST_OUTPUT_DIR}/OpeningByReconstructionImageFilterTestSubtract2.png)
itk_add_test(NAME itkDoubleThresholdImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
  --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/DoubleThresholdImageFilterTest.png
            ${ITK_TEST_OUTPUT_DIR}/DoubleThresholdImageFilterTest.png itkDoubleThresholdImageFilterTest
            ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
            ${ITK_TEST_OUTPUT_DIR}/DoubleThresholdImageFilterTest.png 217 246 255 255)
itk_add_test(NAME itkDoubleThresholdImageFilterTest2
      COMMAND ITKMathematicalMorphologyTestDriver
  --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/DoubleThresholdImageFilterTest2.png
            ${ITK_TEST_OUTPUT_DIR}/DoubleThresholdImageFilterTest2.png itkDoubleThresholdImageFilterTest
            ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
            ${ITK_TEST_OUTPUT_DIR}/DoubleThresholdImageFilterTest2.png 150 164 164 180)
itk_add_test(NAME itkRemoveBoundaryObjectsTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/RemoveBoundaryObjectsTest.png
              ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest.png
    itkRemoveBoundaryObjectsTest ${ITK_DATA_ROOT}/Input/Spots.png ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest.png)
itk_add_test(NAME itkRemoveBoundaryObjectsTest2
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/RemoveBoundaryObjectsTest2.png
              ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest2.png
    itkRemoveBoundaryObjectsTest2 ${ITK_DATA_ROOT}/Input/SpotsInverted.png ${ITK_TEST_OUTPUT_DIR}/RemoveBoundaryObjectsTest2.png)
itk_add_test(NAME itkShapedIteratorFromStructuringElementTest
      COMMAND ITKMathematicalMorphologyTestDriver itkShapedIteratorFromStructuringElementTest)
itk_add_test(NAME itkBlackTopHatImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkBlackTopHatImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkBlackTopHatImageFilterTest.png
    itkTopHatImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkBlackTopHatImageFilterTest.png 0 3)
itk_add_test(NAME itkWhiteTopHatImageFilterTest
      COMMAND ITKMathematicalMorphologyTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/itkWhiteTopHatImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkWhiteTopHatImageFilterTest.png
    itkTopHatImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkWhiteTopHatImageFilterTest.png 1 3)
