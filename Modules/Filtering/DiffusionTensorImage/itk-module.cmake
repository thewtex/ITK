set(DOCUMENTATION "This module contains classes intended to generate or procees
diffusion tensor images. In particular you will find here the filter that
computes a tensor image out of a set of gradient images.")

itk_module(ITK-DiffusionTensorImage DEPENDS ITK-ImageFilterBase ITK-SpatialObjects TEST_DEPENDS ITK-TestKernel DESCRIPTION "${DOCUMENTATION}")
