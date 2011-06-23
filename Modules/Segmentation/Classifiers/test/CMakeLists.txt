itk_module_test()
set(ITK_ClassifiersTests
itkClassifiersHeaderTest.cxx
itkScalarImageKmeansImageFilterTest.cxx
itkSupervisedImageClassifierTest.cxx
itkBayesianClassifierImageFilterTest.cxx
itkKmeansModelEstimatorTest.cxx
itkImageClassifierFilterTest.cxx
itkSampleClassifierFilterTest1.cxx
itkSampleClassifierFilterTest2.cxx
itkSampleClassifierFilterTest3.cxx
itkSampleClassifierFilterTest4.cxx
itkSampleClassifierFilterTest5.cxx
itkSampleClassifierFilterTest6.cxx
itkSampleClassifierFilterTest7.cxx
itkScalarImageKmeansImageFilter3DTest.cxx
)

CreateTestDriver(ITK_Classifiers  "${ITK_Classifiers-Test_LIBRARIES}" "${ITK_ClassifiersTests}")

itk_add_test(NAME itkClassifiersHeaderTest
      COMMAND ITK_ClassifiersTestDriver itkClassifiersHeaderTest)
itk_add_test(NAME itkBayesianClassifierImageFilterTest
      COMMAND ITK_ClassifiersTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Algorithms/itkBayesianClassifierImageFilterTestLabelMap.png
              ${ITK_TEST_OUTPUT_DIR}/itkBayesianClassifierImageFilterTestLabelMap.png
    itkBayesianClassifierImageFilterTest ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png ${ITK_TEST_OUTPUT_DIR}/itkBayesianClassifierImageFilterTestLabelMap.png 4 2)
itk_add_test(NAME itkKmeansModelEstimatorTest
      COMMAND ITK_ClassifiersTestDriver itkKmeansModelEstimatorTest)
itk_add_test(NAME itkImageClassifierFilterTest
      COMMAND ITK_ClassifiersTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Statistics/ImageClassifierFilterTestClassifiedImageTest.png
              ${ITK_TEST_OUTPUT_DIR}/ImageClassifierFilterTestClassifiedImage.png
    itkImageClassifierFilterTest ${ITK_TEST_OUTPUT_DIR}/ImageClassifierFilterTestClassifiedImage.png)
itk_add_test(NAME itkSampleClassifierFilterTest1
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest1)
itk_add_test(NAME itkSampleClassifierFilterTest2
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest2)
itk_add_test(NAME itkSampleClassifierFilterTest3
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest3)
itk_add_test(NAME itkSampleClassifierFilterTest4
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest4)
itk_add_test(NAME itkSampleClassifierFilterTest5
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest5)
itk_add_test(NAME itkSampleClassifierFilterTest6
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest6)
itk_add_test(NAME itkSampleClassifierFilterTest7
      COMMAND ITK_ClassifiersTestDriver itkSampleClassifierFilterTest7
              ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussianTesting.dat)
itk_add_test(NAME itkSupervisedImageClassifierTest
      COMMAND ITK_ClassifiersTestDriver itkSupervisedImageClassifierTest)
itk_add_test(NAME itkScalarImageKmeansImageFilter3DTest
      COMMAND ITK_ClassifiersTestDriver
    --compare ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
              ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz
    itkScalarImageKmeansImageFilter3DTest ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1UCharRaw.nii.gz ${ITK_EXAMPLE_DATA_ROOT}/KmeansTest_T1RawSkullStrip.nii.gz ${ITK_TEST_OUTPUT_DIR}/KmeansTest_T1KmeansPrelimSegmentation.nii.gz)
