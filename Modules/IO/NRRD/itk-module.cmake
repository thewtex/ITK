set(DOCUMENTATION "This module contains an ImageIO class to read and write the
<a href=\"http://teem.sourceforge.net/nrrd/format.html/\">Nearly Raw Raster Data
(Nrrd)</a> medical image format.")

itk_module(ITKIONRRD
  ENABLE_SHARED
  FACTORIES
    NrrdImageIO
  DEPENDS
    ITKNrrdIO
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
