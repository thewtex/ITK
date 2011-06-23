set(ITK_SpatialObjects_SRC
itkCylinderSpatialObject.cxx
itkSpatialObjectFactoryBase.cxx
)

add_library(ITK_SpatialObjects ${ITK_SpatialObjects_SRC})
target_link_libraries(ITK_SpatialObjects  ${ITK_ImageFunction_LIBRARIES} ${ITK_Mesh_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_SpatialObjects)
