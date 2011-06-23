itk_module_test()
set(ITK_ImageAdaptorsTests
itkImageAdaptorTest.cxx
itkImageAdaptorsHeaderTest.cxx
itkRGBToVectorImageAdaptorTest.cxx
itkVectorImageTest.cxx
)

CreateTestDriver(ITK_ImageAdaptors  "${ITK_ImageAdaptors-Test_LIBRARIES}" "${ITK_ImageAdaptorsTests}")

itk_add_test(NAME itkImageAdaptorsHeaderTest
      COMMAND ITK_ImageAdaptorsTestDriver itkImageAdaptorsHeaderTest)
itk_add_test(NAME itkImageAdaptorTest
      COMMAND ITK_ImageAdaptorsTestDriver itkImageAdaptorTest)
itk_add_test(NAME itkRGBToVectorImageAdaptorTest
      COMMAND ITK_ImageAdaptorsTestDriver itkRGBToVectorImageAdaptorTest)
itk_add_test(NAME itkVectorImageTest
      COMMAND ITK_ImageAdaptorsTestDriver itkVectorImageTest
              ${ITK_TEST_OUTPUT_DIR}/VectorImage.nrrd ${ITK_TEST_OUTPUT_DIR}/VectorImage.mhd)
