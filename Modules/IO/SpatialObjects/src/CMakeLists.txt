set(ITK_IO_SpatialObjects_SRC
itkPolygonGroupSpatialObjectXMLFile.cxx
)

add_library(ITK_IO_SpatialObjects ${ITK_IO_SpatialObjects_SRC})
target_link_libraries(ITK_IO_SpatialObjects  ${ITK_SpatialObjects_LIBRARIES} ${ITK_IO_XML_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_SpatialObjects)
