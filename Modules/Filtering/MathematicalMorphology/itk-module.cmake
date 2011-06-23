set(DOCUMENTATION "This module contains classes that implement variations of
mathematical morphology techniques. In addition to the classical erosion,
dilation, opening and closing filters, you will find here geodesic operations,
maxima and minima filters, and reconstruction filters. This module contains
filters for both binary and grayscale mathematical morphology.")

itk_module(ITK_MathematicalMorphology
  DEPENDS
    ITK_ImageIntensity
    ITK_ImageGrid
    ITK_ConnectedComponents
    ITK_IO_PNG
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
