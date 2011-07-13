itk_module(ITKStatistics
  DEPENDS
    ITKCommon
    ITK-Netlib
  TEST_DEPENDS
    ITKTestKernel
    ITKImageIntensity
    ITKImageCompose
)

# Extra test dependency of ImageIntensity is introduced by itkImageToListSampleAdaptorTest.cxx.
