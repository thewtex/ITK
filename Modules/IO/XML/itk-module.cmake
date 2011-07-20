set(DOCUMENTATION "This module contains classes for reading and writing XML
files with ITK LightProcessObjects.")

itk_module(ITKIOXML
  DEPENDS
    ITKExpat
    ITKIOBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
