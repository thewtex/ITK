itk_module(ITKIOMeta
  DEPENDS
    ITK-MetaIO
    ITKIOBase
  TEST_DEPENDS
    ITKTestKernel
    ITKSmoothing
)

# Extra test dependency of ITKSmoothing is caused by itkMetaStreamingIOTest.
