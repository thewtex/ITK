set(DOCUMENTATION "This modules provides classes for comparing images. They are
typically used for comparing two images before and after registration, to
perform regression testing or to compare the outcome of segmentations. In
particular you will find here: checkerboard filter, absolute value differences,
similarity index and STAPLE.")

itk_module(ITK_ImageCompare
  DEPENDS
    ITK_ImageIntensity
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
