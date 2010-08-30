
set( testMeta1Utils_SRCS testMeta1Utils.cxx )
add_executable( testMeta1Utils ${testMeta1Utils_SRCS} )
target_link_libraries( testMeta1Utils ${METAIO_NAMESPACE} )

set( testMeta2Object_SRCS testMeta2Object.cxx )
add_executable( testMeta2Object ${testMeta2Object_SRCS} )
target_link_libraries( testMeta2Object ${METAIO_NAMESPACE} )

set( testMeta3Image_SRCS testMeta3Image.cxx )
add_executable( testMeta3Image ${testMeta3Image_SRCS} )
target_link_libraries( testMeta3Image ${METAIO_NAMESPACE} )

set( testMeta4Tube_SRCS testMeta4Tube.cxx )
add_executable( testMeta4Tube ${testMeta4Tube_SRCS} )
target_link_libraries( testMeta4Tube ${METAIO_NAMESPACE} )

set( testMeta5Blob_SRCS testMeta5Blob.cxx )
add_executable( testMeta5Blob ${testMeta5Blob_SRCS} )
target_link_libraries( testMeta5Blob ${METAIO_NAMESPACE} )

set( testMeta6Surface_SRCS testMeta6Surface.cxx )
add_executable( testMeta6Surface ${testMeta6Surface_SRCS} )
target_link_libraries( testMeta6Surface ${METAIO_NAMESPACE} )

set( testMeta7Line_SRCS testMeta7Line.cxx )
add_executable( testMeta7Line ${testMeta7Line_SRCS} )
target_link_libraries( testMeta7Line ${METAIO_NAMESPACE} )

set( testMeta8Scene_SRCS testMeta8Scene.cxx )
add_executable( testMeta8Scene ${testMeta8Scene_SRCS} )
target_link_libraries( testMeta8Scene ${METAIO_NAMESPACE} )

set( testMeta9Landmark_SRCS testMeta9Landmark.cxx )
add_executable( testMeta9Landmark ${testMeta9Landmark_SRCS} )
target_link_libraries( testMeta9Landmark ${METAIO_NAMESPACE} )

set( testMeta10Contour_SRCS testMeta10Contour.cxx )
add_executable( testMeta10Contour ${testMeta10Contour_SRCS} )
target_link_libraries( testMeta10Contour ${METAIO_NAMESPACE} )

set( testMeta11Form_SRCS testMeta11Form.cxx )
add_executable( testMeta11Form ${testMeta11Form_SRCS} )
target_link_libraries( testMeta11Form ${METAIO_NAMESPACE} )

set( testMeta12Array_SRCS testMeta12Array.cxx )
add_executable( testMeta12Array ${testMeta12Array_SRCS} )
target_link_libraries( testMeta12Array ${METAIO_NAMESPACE} )
