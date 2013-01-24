set(DOCUMENTATION "This module contains the third party <a
href=\"http://dicom.offis.de/dcmtk/\">DCMTK</a> DCMTK is a collection of libraries and applications implementing large parts the DICOM standard.")

itk_module(ITKIODCMTK
  DEPENDS
    ITKDCMTK
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
    ITKImageIntensity
  DESCRIPTION
    "${DOCUMENTATION}"
)
