set(DOCUMENTATION "This module contains classes intended to
interconnect ITK and VNL. It is common to combine these two
to enable ITK to utilize the numerical routines operating on
VNL vectors and matrices.  In particular, these classes convert
between ITK images and VNL matrices and can be extended to
enable other such conversions.")

itk_module(ITKVnlBridge
  DEPENDS
    ITKCommon
  DESCRIPTION
    "${DOCUMENTATION}"
)
