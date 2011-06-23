set(DOCUMENTATION "This module groups image filters whose operations are
related to manipulations of the underlying image grid. For example, flipping an
image, permuting axis, padding, cropping, pasting, tiling, resampling,
shrinking, and changing its origin or spacing or orientation.")

itk_module(ITK_ImageGrid
  DEPENDS
    ITK_ImageFunction
    ITK_ImageFilterBase
  TEST_DEPENDS
    ITK_TestKernel
    ITK_RegistrationCommon
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra test dependency on ITK_RegistrationCommon is introduced by itkShrinkImagePreserveObjectPhysicalLocations.
