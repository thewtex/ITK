itk_module_test()
set(ITKFEMTests
itkFEMHeaderTest.cxx
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

CreateTestDriver(ITKFEM  "${ITKFEM-Test_LIBRARIES}" "${ITKFEMTests}")

itk_add_test(NAME itkFEMHeaderTest
      COMMAND ITKFEMTestDriver itkFEMHeaderTest)
itk_add_test(NAME itkFEMElement2DMembraneTest
      COMMAND ITKFEMTestDriver itkFEMElement2DMembraneTest)
itk_add_test(NAME itkFEMElement2DQuadraticTriangularTest
      COMMAND ITKFEMTestDriver itkFEMElement2DQuadraticTriangularTest)
itk_add_test(NAME itkFEMElement2DStrainTest
      COMMAND ITKFEMTestDriver itkFEMElement2DStrainTest)
itk_add_test(NAME itkFEMElement3DMembraneTest
      COMMAND ITKFEMTestDriver itkFEMElement3DMembraneTest)
itk_add_test(NAME itkFEMC0HexahedralElement-NodalLoads-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/hexa2.fem)
itk_add_test(NAME itkFEMC0HexahedralElement-NoLoads
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/hexa3.fem)
itk_add_test(NAME itkFEMC0HexahedralElement-GravityLoad-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/hexa4-grav.fem)
itk_add_test(NAME itkFEMC0QuadElement-NodalLoads-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/quad2-small.fem)
itk_add_test(NAME itkFEMC0QuadElement-Strain
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/quad2-strain.fem)
itk_add_test(NAME itkFEMC0QuadElement-NoLoads
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/quad4.fem)
itk_add_test(NAME itkFEMC0QuadElement-GravityLoad-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/quad6-grav.fem)
itk_add_test(NAME itkFEMLoadLandmarkImplementation
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/quad-lm.fem)
itk_add_test(NAME itkFEMC0TetrahedralElement-NodalLoads-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tetra2.fem)
itk_add_test(NAME itkFEMC0TetrahedralElement-NoLoads
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tetra3.fem)
itk_add_test(NAME itkFEMC0TetrahedralElement-GravityLoad-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tetra4-grav.fem)
itk_add_test(NAME itkFEMC0QuadElementTrapezoidalGeometry-NoLoads
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/trapezoid.fem)
itk_add_test(NAME itkFEMC0TriangularElement-NodalLoads-BCs
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tri2.fem)
itk_add_test(NAME itkFEMC0TriangularElement-NoLoads
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tri3.fem)
itk_add_test(NAME itkFEMC0TriangularElement-Strain
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tri3-e.fem)
itk_add_test(NAME itkFEMC0TriangularElement-Quadratic
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/tri3-q.fem)
itk_add_test(NAME itkFEMTruss
      COMMAND ITKFEMTestDriver itkFEMElementTest
              ${ITK_DATA_ROOT}/Input/FEM/truss.fem)
itk_add_test(NAME itkFEMExceptionTest
      COMMAND ITKFEMTestDriver itkFEMExceptionTest)
itk_add_test(NAME itkFEMGenerateMeshTest
      COMMAND ITKFEMTestDriver itkFEMGenerateMeshTest)
itk_add_test(NAME itkFEMLinearSystemWrapperDenseVNLTest
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperDenseVNLTest)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest1
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              0)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest2
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              1)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest3
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              2)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest4
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              3)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest5
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              4)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest6
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              5)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest7
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest
              6)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest8
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              0)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest9
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              1)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest10
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              2)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest11
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              3)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest12
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              4)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest13
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              5)
itk_add_test(NAME itkFEMLinearSystemWrapperItpackTest14
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperItpackTest2
              6)
itk_add_test(NAME itkFEMLinearSystemWrapperVNLTest
      COMMAND ITKFEMTestDriver itkFEMLinearSystemWrapperVNLTest)
itk_add_test(NAME itkFEMPArrayTest
      COMMAND ITKFEMTestDriver itkFEMPArrayTest)
