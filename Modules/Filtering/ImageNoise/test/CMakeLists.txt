itk_module_test()
set(ITKImageNoiseTests
  itkAdditiveGaussianNoiseImageFilterTest2.cxx
  itkShotNoiseImageFilterTest2.cxx
  itkSpeckleNoiseImageFilterTest2.cxx
  itkSaltAndPepperNoiseImageFilterTest2.cxx
  itkPeakSignalToNoiseRatioCalculatorTest.cxx
)


CreateTestDriver(ITKImageNoise  "${ITKImageNoise-Test_LIBRARIES}" "${ITKImageNoiseTests}")

itk_add_test(NAME itkAdditiveGaussianNoiseImageFilterTest2
  COMMAND ITKImageNoiseTestDriver
  itkAdditiveGaussianNoiseImageFilterTest2
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
   ${ITK_TEST_OUTPUT_DIR}/itkAdditiveGaussianNoiseImageFilterTest2.png
   29.85 0)
itk_add_test(NAME itkAdditiveGaussianNoiseImageFilterTest2PSNR
  COMMAND ITKImageNoiseTestDriver
  itkPeakSignalToNoiseRatioCalculatorTest
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
   ${ITK_TEST_OUTPUT_DIR}/itkAdditiveGaussianNoiseImageFilterTest2.png
   20 1)
set_tests_properties(itkAdditiveGaussianNoiseImageFilterTest2PSNR
  PROPERTIES
   DEPENDS itkAdditiveGaussianNoiseImageFilterTest2)
itk_add_test(NAME itkShotNoiseImageFilterTest2
  COMMAND ITKImageNoiseTestDriver
  itkShotNoiseImageFilterTest2
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
      ${ITK_TEST_OUTPUT_DIR}/itkShotNoiseImageFilterTest2.png
      0.105 )
itk_add_test(NAME itkShotNoiseImageFilterTest2PSNR
  COMMAND ITKImageNoiseTestDriver
  itkPeakSignalToNoiseRatioCalculatorTest
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
   ${ITK_TEST_OUTPUT_DIR}/itkShotNoiseImageFilterTest2.png
   20 1)
set_tests_properties(itkShotNoiseImageFilterTest2PSNR
  PROPERTIES
   DEPENDS itkShotNoiseImageFilterTest2)
itk_add_test(NAME itkSpeckleNoiseImageFilterTest2
  COMMAND ITKImageNoiseTestDriver
  itkSpeckleNoiseImageFilterTest2
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
      ${ITK_TEST_OUTPUT_DIR}/itkSpeckleNoiseImageFilterTest2.png
      0.261)
itk_add_test(NAME itkSpeckleNoiseImageFilterTest2PSNR
  COMMAND ITKImageNoiseTestDriver
  itkPeakSignalToNoiseRatioCalculatorTest
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
   ${ITK_TEST_OUTPUT_DIR}/itkSpeckleNoiseImageFilterTest2.png
   20 1)
set_tests_properties(itkSpeckleNoiseImageFilterTest2PSNR
  PROPERTIES
   DEPENDS itkSpeckleNoiseImageFilterTest2)
itk_add_test(NAME itkSaltAndPepperNoiseImageFilterTest2
  COMMAND ITKImageNoiseTestDriver
  itkSaltAndPepperNoiseImageFilterTest2
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
      ${ITK_TEST_OUTPUT_DIR}/itkSaltAndPepperNoiseImageFilterTest2.png
      0.0257)
itk_add_test(NAME itkSaltAndPepperNoiseImageFilterTest2PSNR
  COMMAND ITKImageNoiseTestDriver
  itkPeakSignalToNoiseRatioCalculatorTest
   DATA{${ITK_DATA_ROOT}/Input/cthead1.png}
   ${ITK_TEST_OUTPUT_DIR}/itkSaltAndPepperNoiseImageFilterTest2.png
   20 1)
set_tests_properties(itkSaltAndPepperNoiseImageFilterTest2PSNR
  PROPERTIES
   DEPENDS itkSaltAndPepperNoiseImageFilterTest2)
