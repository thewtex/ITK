itk_module(ITK-FastMarching DEPENDS
ITK-Common
ITK-QuadEdgeMesh
ITK-QuadEdgeMeshFiltering # cause of QuadEdgeMeshToQuadEdgeMeshFilter
ITK-ConnectedComponents
### test ###
TEST_DEPENDS
ITK-IO-NIFTI
ITK-Review # cause of LabelContourImageFilter
ITK-TestKernel
)
