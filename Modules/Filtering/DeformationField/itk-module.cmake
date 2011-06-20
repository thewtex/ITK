set(DOCUMENTATION "This module contains filters for computing and manipulating
deformation fields. In ITK a deformation field is usually represented as an
image of vectors. That is, every pixel contains a vector. In this module you
will find filters for: computing a deformation field from a transform,
computing inverse deformation fields, and computing the Jacobian Determinant of
a deformation field.")

itk_module(ITK-DeformationField
  DEPENDS
    ITK-Transform
    ITK-ImageGrid
    ITK-ImageIntensity
  TEST_DEPENDS
    ITK-TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
