set(DOCUMENTATION "This module contains classes that provide an interface bewteen ITK and VTK.")

itk_module(ITK-ItkVtkGlue
  DEPENDS
    ITK-Common
    ITK-ImageIntensity
    ITK-ImageAdaptors
    ITK-ImageGrid
    ITK-VTK
  TEST_DEPENDS
    ITK-TestKernel
    ITK-Smoothing
  EXCLUDE_FROM_ALL
  DESCRIPTION
    "${DOCUMENTATION}")

# extra test dependency on Smoothing is introduced by itkVtkMedianImagefilterTest.
