set(ITK_FFT_SRC
itkFFTWGlobalConfiguration.cxx
)

add_library(ITK_FFT ${ITK_FFT_SRC})
target_link_libraries(ITK_FFT  ${ITK_Common_LIBRARIES})

if(USE_FFTWD)
  add_dependencies(ITK_FFT fftwd)
  target_link_libraries(ITK_FFT fftw3 fftw3_threads)
endif(USE_FFTWD)
if(USE_FFTWF)
  add_dependencies(ITK_FFT fftwf)
  target_link_libraries(ITK_FFT fftw3f fftw3f_threads )
endif(USE_FFTWF)

itk_module_target(ITK_FFT)
