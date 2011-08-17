itk_module_test()
set(ITKFFTTests
itkFFTCyclicShiftImageFilterTest.cxx
itkVnlFFTTest.cxx
)

if (USE_FFTWF)
  set( ITKFFTTests ${ITKFFTTests} itkFFTWF_FFTTest.cxx itkVnlFFTWF_FFTTest.cxx)
endif()

if (USE_FFTWD)
  set( ITKFFTTests ${ITKFFTTests} itkFFTWD_FFTTest.cxx itkVnlFFTWD_FFTTest.cxx)
endif()


CreateTestDriver(ITKFFT  "${ITKFFT-Test_LIBRARIES}" "${ITKFFTTests}")

itk_add_test(NAME itkVnlFFTTest
      COMMAND ITKFFTTestDriver itkVnlFFTTest)

if(USE_FFTWF)
  itk_add_test(NAME itkFFTWF_FFTTest
    COMMAND ITKFFTTestDriver itkFFTWF_FFTTest ${ITK_TEST_OUTPUT_DIR} )
  itk_add_test(NAME itkVnlFFTWF_FFTTest
    COMMAND ITKFFTTestDriver  itkVnlFFTWF_FFTTest )
  set_tests_properties(itkVnlFFTWF_FFTTest PROPERTIES ENVIRONMENT
    "ITK_FFTW_READ_WISDOM_CACHE=oN;ITK_FFTW_WRITE_WISDOM_CACHE=oN;ITK_FFTW_WISDOM_CACHE_FILE=${ITK_TEST_OUTPUT_DIR}/.wisdom_from_ITK_FFTW_WISDOM_CACHE_FILE;ITK_FFTW_PLAN_RIGOR=FFTW_EXHAUSTIVE")
endif(USE_FFTWF)

if(USE_FFTWD)
  itk_add_test(NAME itkFFTWD_FFTTest
    COMMAND ITKFFTTestDriver itkFFTWD_FFTTest ${ITK_TEST_OUTPUT_DIR} )

  itk_add_test(NAME itkVnlFFTWD_FFTTest
    COMMAND  ITKFFTTestDriver  itkVnlFFTWD_FFTTest)
  set_tests_properties(itkVnlFFTWD_FFTTest PROPERTIES ENVIRONMENT
    "ITK_FFTW_READ_WISDOM_CACHE=oN;ITK_FFTW_WISDOM_CACHE_BASE=${ITK_TEST_OUTPUT_DIR};ITK_FFTW_PLAN_RIGOR=FFTW_EXHAUSTIVE;ITK_FFTW_WRITE_WISDOM_CACHE=oN")
endif(USE_FFTWD)

itk_add_test(NAME itkFFTCyclicShiftImageFilterTestOdd0
      COMMAND ITKFFTTestDriver
    --compare DATA{Baseline/itkFFTShiftImageFilterTest0.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTest0.png
    itkFFTCyclicShiftImageFilterTest ${ITK_DATA_ROOT}/Input/BrainSliceBinary.png ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTest0.png 0)
itk_add_test(NAME itkFFTCyclicShiftImageFilterTestOdd1
      COMMAND ITKFFTTestDriver
    --compare ${ITK_DATA_ROOT}/Input/BrainSliceBinary.png
              ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTest1.png
    itkFFTCyclicShiftImageFilterTest DATA{Baseline/itkFFTShiftImageFilterTest0.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTest1.png 1)
itk_add_test(NAME itkFFTCyclicShiftImageFilterTestEven0
      COMMAND ITKFFTTestDriver
    --compare DATA{Baseline/itkFFTShiftImageFilterTestEven0.png}
              ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTestEven0.png
    itkFFTCyclicShiftImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTestEven0.png 0)
itk_add_test(NAME itkFFTCyclicShiftImageFilterTestEven1
      COMMAND ITKFFTTestDriver
    --compare ${ITK_DATA_ROOT}/Input/cthead1.png
              ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTestEven1.png
    itkFFTCyclicShiftImageFilterTest DATA{Baseline/itkFFTShiftImageFilterTestEven0.png} ${ITK_TEST_OUTPUT_DIR}/itkFFTCyclicShiftImageFilterTestEven1.png 1)
