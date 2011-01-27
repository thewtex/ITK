if( NOT ITK_DISABLE_CXX_TESTING )
if(ITK_BINARY_DIR)
  if(TCL_TCLSH)
    add_test(PrintSelf-FEM ${TCL_TCLSH}
             ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl
             ${ITK_SOURCE_DIR}/Code/Numerics/FEM)
  endif(TCL_TCLSH)
endif(ITK_BINARY_DIR)



set(KIT FEM)
set(CXX_TESTS ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)

set(KIT_LIBS ITKNumerics ITKFEM)
set(KitTests
  itkFEMElement2DMembraneTest.cxx
  itkFEMElement2DQuadraticTriangularTest.cxx
  itkFEMElement2DStrainTest.cxx
  itkFEMElement3DMembraneTest.cxx
  itkFEMElementTest.cxx
  itkFEMExceptionTest.cxx
  itkFEMGenerateMeshTest.cxx
  itkFEMLinearSystemWrapperDenseVNLTest.cxx
  itkFEMLinearSystemWrapperItpackTest.cxx
  itkFEMLinearSystemWrapperItpackTest2.cxx
  itkFEMLinearSystemWrapperVNLTest.cxx
  itkFEMPArrayTest.cxx
)

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkv4TestMainAfter.inc\"")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KitTests}
  EXTRA_INCLUDE itkv4TestMain.h
  FUNCTION ProcessArguments
  )

add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT_LIBS})

add_test(itkFEMC0HexahedralElement-NodalLoads-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/hexa2.fem)
add_test(itkFEMC0HexahedralElement-NoLoads ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/hexa3.fem)
add_test(itkFEMC0HexahedralElement-GravityLoad-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/hexa4-grav.fem)
add_test(itkFEMC0QuadElement-NodalLoads-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad2-small.fem)
add_test(itkFEMC0QuadElement-Strain ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad2-strain.fem)
add_test(itkFEMC0QuadElement-NoLoads ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad4.fem)
add_test(itkFEMC0QuadElement-GravityLoad-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad6-grav.fem)
add_test(itkFEMLoadLandmarkImplementation ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad-lm.fem)

# Edge loads are not re-implemented yet
#add_test(itkFEMC0QuadElement-EdgeLoad-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/quad_1_edge.fem)

add_test(itkFEMC0TetrahedralElement-NodalLoads-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tetra2.fem)
add_test(itkFEMC0TetrahedralElement-NoLoads ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tetra3.fem)
add_test(itkFEMC0TetrahedralElement-GravityLoad-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tetra4-grav.fem)
add_test(itkFEMC0QuadElementTrapezoidalGeometry-NoLoads ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/trapezoid.fem)
add_test(itkFEMC0TriangularElement-NodalLoads-BCs ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tri2.fem)
add_test(itkFEMC0TriangularElement-NoLoads ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tri3.fem)
add_test(itkFEMC0TriangularElement-Strain ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tri3-e.fem)
add_test(itkFEMC0TriangularElement-Quadratic ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/tri3-q.fem)
add_test(itkFEMTruss ${CXX_TESTS} itkFEMElementTest ${ITK_DATA_ROOT}/Input/FEM/truss.fem)

add_test(itkFEMLinearSystemWrapperItpackTest  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest)

add_test(itkFEMLinearSystemWrapperItpackTest1  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 0 )
add_test(itkFEMLinearSystemWrapperItpackTest2  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 1 )
add_test(itkFEMLinearSystemWrapperItpackTest3  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 2 )
add_test(itkFEMLinearSystemWrapperItpackTest4  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 3 )
add_test(itkFEMLinearSystemWrapperItpackTest5  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 4 )
add_test(itkFEMLinearSystemWrapperItpackTest6  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 5 )
add_test(itkFEMLinearSystemWrapperItpackTest7  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest 6 )

add_test(itkFEMLinearSystemWrapperItpackTest8   ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 0 )
add_test(itkFEMLinearSystemWrapperItpackTest9   ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 1 )
add_test(itkFEMLinearSystemWrapperItpackTest10  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 2 )
add_test(itkFEMLinearSystemWrapperItpackTest11  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 3 )
add_test(itkFEMLinearSystemWrapperItpackTest12  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 4 )
add_test(itkFEMLinearSystemWrapperItpackTest13  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 5 )
add_test(itkFEMLinearSystemWrapperItpackTest14  ${CXX_TESTS} itkFEMLinearSystemWrapperItpackTest2 6 )

add_test(itkFEMLinearSystemWrapperVNLTest  ${CXX_TESTS} itkFEMLinearSystemWrapperVNLTest)
add_test(itkFEMLinearSystemWrapperDenseVNLTest  ${CXX_TESTS} itkFEMLinearSystemWrapperDenseVNLTest)
add_test(itkFEMPArrayTest  ${CXX_TESTS} itkFEMPArrayTest)

add_test(itkFEMExceptionTest ${CXX_TESTS} itkFEMExceptionTest)
add_test(itkFEMGenerateMeshTest ${CXX_TESTS} itkFEMGenerateMeshTest)
add_test(itkFEMElement2DMembraneTest ${CXX_TESTS} itkFEMElement2DMembraneTest)
add_test(itkFEMElement3DMembraneTest ${CXX_TESTS} itkFEMElement3DMembraneTest)
add_test(itkFEMElement2DStrainTest ${CXX_TESTS} itkFEMElement2DStrainTest)
add_test(itkFEMElement2DQuadraticTriangularTest ${CXX_TESTS} itkFEMElement2DQuadraticTriangularTest)

#add_test(itkFEMBar2DTest ${CXX_TESTS} itkFEMBar2DTest)

endif( NOT ITK_DISABLE_CXX_TESTING )



