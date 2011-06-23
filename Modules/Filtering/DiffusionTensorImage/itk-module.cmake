set(DOCUMENTATION "This module contains classes intended to generate or procees
diffusion tensor images. In particular you will find here the filter that
computes a tensor image from a set of gradient images.")

itk_module(ITK_DiffusionTensorImage
  DEPENDS
    ITK_ImageFeature
    ITK_SpatialObjects
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
