project(MeshExamples)
include_regular_expression("^.*$")

add_executable(PointSet1 PointSet1.cxx )
target_link_libraries(PointSet1 ${ITK_LIBRARIES})

add_executable(PointSet2 PointSet2.cxx )
target_link_libraries(PointSet2 ${ITK_LIBRARIES})

add_executable(PointSet3 PointSet3.cxx )
target_link_libraries(PointSet3 ${ITK_LIBRARIES})

add_executable(RGBPointSet RGBPointSet.cxx )
target_link_libraries(RGBPointSet ${ITK_LIBRARIES})

add_executable(PointSetWithVectors PointSetWithVectors.cxx )
target_link_libraries(PointSetWithVectors ${ITK_LIBRARIES})

add_executable(PointSetWithCovariantVectors PointSetWithCovariantVectors.cxx )
target_link_libraries(PointSetWithCovariantVectors ${ITK_LIBRARIES})

add_executable(Mesh1 Mesh1.cxx )
target_link_libraries(Mesh1 ${ITK_LIBRARIES})

add_executable(Mesh2 Mesh2.cxx )
target_link_libraries(Mesh2 ${ITK_LIBRARIES})

add_executable(Mesh3 Mesh3.cxx )
target_link_libraries(Mesh3 ${ITK_LIBRARIES})

add_executable(MeshTraits MeshTraits.cxx )
target_link_libraries(MeshTraits ${ITK_LIBRARIES})

add_executable(MeshKComplex MeshKComplex.cxx )
target_link_libraries(MeshKComplex ${ITK_LIBRARIES})

add_executable(MeshPolyLine MeshPolyLine.cxx )
target_link_libraries(MeshPolyLine ${ITK_LIBRARIES})

add_executable(MeshCellsIteration MeshCellsIteration.cxx )
target_link_libraries(MeshCellsIteration ${ITK_LIBRARIES})

add_executable(MeshCellVisitor MeshCellVisitor.cxx )
target_link_libraries(MeshCellVisitor ${ITK_LIBRARIES})

add_executable(MeshCellVisitor2 MeshCellVisitor2.cxx )
target_link_libraries(MeshCellVisitor2 ${ITK_LIBRARIES})

add_executable(AutomaticMesh AutomaticMesh.cxx )
target_link_libraries(AutomaticMesh ${ITK_LIBRARIES})

add_executable(ImageToPointSet ImageToPointSet.cxx )
target_link_libraries(ImageToPointSet ${ITK_LIBRARIES})

set(MESH_EXAMPLES MeshExamples)

add_executable(MeshExamples MeshExamples.cxx )
target_link_libraries(MeshExamples ${ITK_LIBRARIES})

if( NOT ITK_DISABLE_CXX_TESTING )

if(BUILD_TESTING)

add_test(NAME AutomaticMeshTest COMMAND MeshExamples AutomaticMeshTest)

add_test(NAME ImageToPointSetTest COMMAND MeshExamples
        ImageToPointSetTest ${ITK_SOURCE_DIR}/Examples/Data/BrainMidSagittalSlice.png)

add_test(NAME Mesh1Test COMMAND MeshExamples Mesh1Test)

add_test(NAME Mesh2Test COMMAND MeshExamples Mesh2Test)

add_test(NAME Mesh3Test COMMAND MeshExamples Mesh3Test)

add_test(NAME MeshCellsIterationTest COMMAND MeshExamples MeshCellsIterationTest)

add_test(NAME MeshCellVisitorTest COMMAND MeshExamples MeshCellVisitorTest)

add_test(NAME MeshCellVisitor2Test COMMAND MeshExamples MeshCellVisitor2Test)

add_test(NAME MeshKComplexTest COMMAND MeshExamples MeshKComplexTest)

add_test(NAME MeshPolyLineTest COMMAND MeshExamples MeshPolyLineTest)

add_test(NAME MeshTraitsTest COMMAND MeshExamples MeshTraitsTest)

add_test(NAME PointSet1Test COMMAND MeshExamples PointSet1Test)

add_test(NAME PointSet2Test COMMAND MeshExamples PointSet2Test)

add_test(NAME PointSet3Test COMMAND MeshExamples PointSet3Test)

add_test(NAME PointSetWithCovariantVectorsTest COMMAND MeshExamples PointSetWithCovariantVectorsTest)

add_test(NAME PointSetWithVectorsTest COMMAND MeshExamples PointSetWithVectorsTest)

add_test(NAME RGBPointSetTest COMMAND MeshExamples RGBPointSetTest)

endif(BUILD_TESTING)

endif( NOT ITK_DISABLE_CXX_TESTING )


