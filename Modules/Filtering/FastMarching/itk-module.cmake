set(DOCUMENTATION "This module contains implementations of generalized versions
of the Fast Marching filter. These implementations cover the use of Fast
Marching in both itk::Images and itk::QuadEdgeMeshes.")

itk_module(ITK_FastMarching
  DEPENDS
    ITK_Common
    ITK_QuadEdgeMesh
    ITK_ConnectedComponents
  TEST_DEPENDS
    ITK_IO_NIFTI
    ITK_ImageLabel
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
