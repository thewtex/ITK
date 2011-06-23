set(DOCUMENTATION "SpatialObjects are intended to represent regions in space.
The basic functionality of a SpatialObject is to answer the question of
whether a physical point is inside or outside of the SpatialObject. They are
commonly used for representing masks in an analytical form, as well as
approximations of shape by combining them into hierarchical structures similar
to scene graphs.")

itk_module(ITK_SpatialObjects
  DEPENDS
    ITK_ImageFunction
    ITK_Mesh
    ITK_IO_Base
  TEST_DEPENDS
    ITK_TestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
