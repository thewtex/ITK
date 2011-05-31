set(DOCUMENTATION "This module contains filters that convolve an image
with a kernel. Convolution is a fundamental operation in many image
analysis algorithms.")

itk_module(ITK-Convolution DEPENDS ITK-ImageGrid TEST_DEPENDS ITK-TestKernel DESCRIPTION "${DOCUMENTATION}")
