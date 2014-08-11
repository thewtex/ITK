project(MeshExamples)
include_regular_expression("^.*$")

foreach(example
    PointSet1
    PointSet2
    PointSet3
    RGBPointSet
    PointSetWithVectors
    PointSetWithCovariantVectors
    Mesh1
    Mesh2
    Mesh3
    MeshTraits
    MeshKComplex
    MeshPolyLine
    MeshCellsIteration
    MeshCellVisitor
    MeshCellVisitor2
    AutomaticMesh
    ImageToPointSet
    )
  add_example(${example})
endforeach()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
