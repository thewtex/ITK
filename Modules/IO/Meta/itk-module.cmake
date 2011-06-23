itk_module(ITK_IO_Meta
  DEPENDS
    ITK_MetaIO
    ITK_IO_Base
  TEST_DEPENDS
    ITK_TestKernel
    ITK_Smoothing
)

# Extra test dependency of ITK_Smoothing is caused by itkMetaStreamingIOTest.
