itk_module_test()
set(ITK-FFTTests
itkFFTHeaderTest.cxx
itkFFTTest.cxx
itkFFTWD_FFTTest.cxx
itkFFTWF_FFTTEST.cxx
itkVnlFFTTest.cxx
itkVnlFFTWD_FFTTest.cxx
itkVnlFFTWF_FFTTest.cxx
)

CreateTestDriver(ITK-FFT  "${ITK-FFT-Test_LIBRARIES}" "${ITK-FFTTests}")

add_test(NAME itkFFTHeaderTest
      COMMAND ITK-FFTTestDriver itkFFTHeaderTest)
