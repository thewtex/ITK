set(DOCUMENTATION "This module contains filters that combine several images
into a single output image. For example, take several input scalar images and
package them into an output image of multiple components.")

itk_module(ITK_ImageCompose
  DEPENDS
    ITK_ImageFilterBase
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
