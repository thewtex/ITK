set(DOCUMENTATION "This module contain ITK ImageIO classes for the Grass Root
DICOM (GDCM) based readers and writers of the medical imaging DICOM standard.")

itk_module(ITKIOGDCM
  DEPENDS
    ITKGDCM
    ITKIOBase
  TEST_DEPENDS
    ITKTestKernel
    ITKImageIntensity
  DESCRIPTION
    "${DOCUMENTATION}"
)
