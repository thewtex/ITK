itk_module(ITK_Statistics
  DEPENDS
    ITK_Common
    ITK_Netlib
  TEST_DEPENDS
    ITK_TestKernel
    ITK_ImageIntensity
    ITK_ImageCompose
)

# Extra test dependency of ImageIntensity is introduced by itkImageToListSampleAdaptorTest.cxx.
