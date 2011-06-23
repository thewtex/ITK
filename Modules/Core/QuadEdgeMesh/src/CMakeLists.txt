set(ITK_QuadEdgeMesh_SRC
itkQuadEdge.cxx
)

add_library(ITK_QuadEdgeMesh ${ITK_QuadEdgeMesh_SRC})
target_link_libraries(ITK_QuadEdgeMesh  ${ITK_Mesh_LIBRARIES})
itk_module_target(ITK_QuadEdgeMesh)
