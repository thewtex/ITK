add_subdirectory(Iterators)
add_subdirectory(DataRepresentation)
add_subdirectory(Filtering)
add_subdirectory(IO)
add_subdirectory(Numerics)
add_subdirectory(Registration)
add_subdirectory(Segmentation)
add_subdirectory(SpatialObjects)
add_subdirectory(Statistics)

if( ITK_USE_REVIEW )
  add_subdirectory( Review )
endif( ITK_USE_REVIEW )

