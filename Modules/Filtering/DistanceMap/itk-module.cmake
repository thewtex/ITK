set(DOCUMENTATION "This module contains multiple implementations of distance
map filters. They include the implementations of Danielsson and Maurer, as well
as other distance concepts such as Hausdorff and Chamfer distances.")

itk_module(ITK_DistanceMap
  DEPENDS
    ITK_ImageIntensity
    ITK_MathematicalMorphology
    ITK_NarrowBand
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
