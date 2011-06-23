set(ITK_Mesh_SRC
itkMeshRegion.cxx
itkSimplexMeshGeometry.cxx
)

add_library(ITK_Mesh ${ITK_Mesh_SRC})
target_link_libraries(ITK_Mesh  ${ITK_Transform_LIBRARIES})
itk_module_target(ITK_Mesh)
