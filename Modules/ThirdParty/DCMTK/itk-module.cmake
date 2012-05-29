set(DOCUMENTATION "This module builds the DCMTK library <a
href=\"http://dicom.offis.de/dcmtk.php.en\">DCMTK</a> DICOM
library suite.")

#
# THIS DOESN'T WORK
# The Module depencies don't force compiling before
# DCMTK, the important <module>_include_dir, <module>_libraries don't work.
itk_module(ITKDCMTK
  DEPENDS
    ITKJPEG
    ITKTIFF
    ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
)
