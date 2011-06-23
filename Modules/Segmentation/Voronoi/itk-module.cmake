itk_module(ITK_Voronoi
  DEPENDS
    ITK_ImageFilterBase
    ITK_Mesh
  TEST_DEPENDS
    ITK_TestKernel
    ITK_Smoothing
)

# Extra dependency on Smoothing is introduced by itkVoronoiPartitioningImageFilterTest.
