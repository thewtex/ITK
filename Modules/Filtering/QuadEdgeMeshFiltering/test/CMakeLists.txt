itk_module_test()
set(ITKQuadEdgeMeshFilteringTests
itkAutomaticTopologyQuadEdgeMeshSourceTest.cxx
itkBorderQuadEdgeMeshFilterTest.cxx
itkBinaryMask3DQuadEdgeMeshSourceTest.cxx
itkCleanQuadEdgeMeshFilterTest.cxx
itkCriterionCellSubdivisionQuadEdgeMeshFilterTest.cxx
itkCriterionEdgeCellSubdivisionQuadEdgeMeshFilterTest.cxx
itkDelaunayConformingQuadEdgeMeshFilterTest.cxx
itkDiscreteGaussianCurvatureQuadEdgeMeshFilterTest.cxx
itkDiscreteMaximumCurvatureQuadEdgeMeshFilterTest.cxx
itkDiscreteMeanCurvatureQuadEdgeMeshFilterTest.cxx
itkDiscreteMinimumCurvatureQuadEdgeMeshFilterTest.cxx
itkEdgeCellSubdivisionQuadEdgeMeshFilterTest.cxx
itkNormalQuadEdgeMeshFilterTest.cxx
itkParameterizationQuadEdgeMeshFilterTest.cxx
itkQuadricDecimationQuadEdgeMeshFilterTest.cxx
itkRegularSphereQuadEdgeMeshSourceTest.cxx
itkSmoothingQuadEdgeMeshFilterTest.cxx
itkSquaredEdgeLengthDecimationQuadEdgeMeshFilterTest.cxx
itkSubdivisionQuadEdgeMeshFilterTest.cxx
)

CreateTestDriver(ITKQuadEdgeMeshFiltering  "${ITKQuadEdgeMeshFiltering-Test_LIBRARIES}" "${ITKQuadEdgeMeshFilteringTests}")

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

itk_add_test(NAME itkBorderQuadEdgeMeshFilterTest00
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkBorderQuadEdgeMeshFilterTest
            DATA{${INPUTDATA}/mushroom.vtk} 0 0 ${TEMP}/border00.vtk
)

itk_add_test(NAME itkBorderQuadEdgeMeshFilterTest01
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkBorderQuadEdgeMeshFilterTest
            DATA{${INPUTDATA}/mushroom.vtk} 0 1 ${TEMP}/border01.vtk
)

itk_add_test(NAME itkBorderQuadEdgeMeshFilterTest10
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkBorderQuadEdgeMeshFilterTest
            DATA{${INPUTDATA}/mushroom.vtk} 1 0 ${TEMP}/border10.vtk
)

itk_add_test(NAME itkBorderQuadEdgeMeshFilterTest11
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkBorderQuadEdgeMeshFilterTest
            DATA{${INPUTDATA}/mushroom.vtk} 1 1 ${TEMP}/border11.vtk
)

itk_add_test(NAME itkSquaredEdgeLengthDecimationQuadEdgeMeshFilterTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
          itkSquaredEdgeLengthDecimationQuadEdgeMeshFilterTest
          DATA{${INPUTDATA}/mushroom.vtk} 20 ${TEMP}/temp_SquaredEdgeLengthDecimationResult1.vtk)

itk_add_test(NAME itkSmoothingQuadEdgeMeshFilterTest0
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
          itkSmoothingQuadEdgeMeshFilterTest
          DATA{${INPUTDATA}/genusZeroSurface01.vtk} 10 0.1 0 ${TEMP}/temp_SmoothResult0.vtk)

itk_add_test(NAME itkSmoothingQuadEdgeMeshFilterTest1
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
             itkSmoothingQuadEdgeMeshFilterTest
          DATA{${INPUTDATA}/genusZeroSurface01.vtk} 10 0.1 1 ${TEMP}/temp_SmoothResult1.vtk)

set( CURV_TESTS Gaussian Maximum Mean Minimum )
foreach( loop_var ${CURV_TESTS} )
  itk_add_test(NAME itkDiscrete${loop_var}CurvatureQuadEdgeMeshFilterTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
            itkDiscrete${loop_var}CurvatureQuadEdgeMeshFilterTest
            DATA{${INPUTDATA}/mushroom.vtk})
endforeach()

itk_add_test(NAME itkDelaunayConformingQuadEdgeMeshFilterTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
          itkDelaunayConformingQuadEdgeMeshFilterTest DATA{${INPUTDATA}/mushroom.vtk} ${TEMP}/mushrom_delaunay.vtk)
itk_add_test(NAME itkCleanQuadEdgeMeshFilterTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
          itkCleanQuadEdgeMeshFilterTest
          DATA{${INPUTDATA}/mushroom.vtk} 0.1 ${TEMP}/temp_CleanResult1.vtk)
 itk_add_test(NAME itkQuadricDecimationQuadEdgeMeshFilterTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkQuadricDecimationQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/mushroom.vtk} 100 ${TEMP}/temp_QuadricDecimationResult1.vtk)
itk_add_test(NAME itkQuadEdgeMeshQuadricDecimationTetrahedronTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver
              itkQuadricDecimationQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/tetrahedron.vtk} 2 ${TEMP}/temp_QuadricDecimationTetrahedron.vtk)
itk_add_test(NAME itkAutomaticTopologyQuadEdgeMeshSourceTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkAutomaticTopologyQuadEdgeMeshSourceTest)
itk_add_test(NAME itkBinaryMask3DQuadEdgeMeshSourceTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkBinaryMask3DQuadEdgeMeshSourceTest)
itk_add_test(NAME itkRegularSphereQuadEdgeMeshSourceTest
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkRegularSphereQuadEdgeMeshSourceTest
              ${ITK_TEST_OUTPUT_DIR}/itkRegularSphereMeshQuadEdgeMeshSourceTest.vtk)
itk_add_test(NAME itkNormalQuadEdgeMeshFilterTest0
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkNormalQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/mushroom.vtk} 0)
itk_add_test(NAME itkNormalQuadEdgeMeshFilterTest1
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkNormalQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/mushroom.vtk} 1)
itk_add_test(NAME itkNormalQuadEdgeMeshFilterTest2
      COMMAND ITKQuadEdgeMeshFilteringTestDriver itkNormalQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/mushroom.vtk} 2)

foreach( border 0 1 )
  foreach( coeff 0 1 2 3 4 )
    foreach( solver 0 1 )
      itk_add_test(NAME itkParameterizationQuadEdgeMeshFilterTestb${border}_c${coeff}_s${solver}
            COMMAND ITKQuadEdgeMeshFilteringTestDriver
                    itkParameterizationQuadEdgeMeshFilterTest
              DATA{${INPUTDATA}/mushroom.vtk} ${border} ${coeff} ${solver}
              ${TEMP}/param_border${border}_coeff${coeff}_solver${solver}.vtk)
    endforeach()
  endforeach()
endforeach()

itk_add_test(NAME itkModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_butterfly_0.vtk 0 1)

itk_add_test(NAME itkModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_butterfly_1.vtk 0 2)

itk_add_test(NAME itkLinearTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_linear_0.vtk 1 1)

itk_add_test(NAME itkLinearTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_linear_1.vtk 1 2)

itk_add_test(NAME itkLoopTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_loop_0.vtk 2 1)

itk_add_test(NAME itkLoopTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_loop_1.vtk 2 2)


itk_add_test(NAME itkSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_square3_0.vtk 3 1)

itk_add_test(NAME itkSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_square3_1.vtk 3 2)

itk_add_test(NAME itkAdaptiveModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_butterfly_0.vtk 0 1 1)

itk_add_test(NAME itkAdaptiveModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_butterfly_1.vtk 0 2 1)

itk_add_test(NAME itkAdaptiveLinearTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_linear_0.vtk 1 1 1)

itk_add_test(NAME itkAdaptiveLinearTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_linear_1.vtk 1 2 1)

itk_add_test(NAME itkAdaptiveLoopTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_loop_0.vtk 2 1 1)

itk_add_test(NAME itkAdaptiveLoopTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_loop_1.vtk 2 2 1)

itk_add_test(NAME itkAdaptiveSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilterTest0
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_square3_0.vtk 3 1 1)

itk_add_test(NAME itkAdaptiveSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilterTest1
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_adaptive_square_1.vtk 3 2 1)

itk_add_test(NAME itkCriterionButterflyTriangleCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_butterfly.vtk 0 0.01)

itk_add_test(NAME itkCriterionLinearTriangleCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_linear.vtk 1 0.01)

itk_add_test(NAME itkCriterionLoopTriangleCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_loop.vtk 2 0.01)

itk_add_test(NAME itkCriterionSquareTriangleCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_square3.vtk 3 0.01)

itk_add_test(NAME itkButterflyTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_edge_butterfly.vtk 0 0.1)

itk_add_test(NAME itkLinearTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_edge_linear.vtk 1 0.1)

itk_add_test(NAME itkLoopTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_edge_loop.vtk 2 0.01)

itk_add_test(NAME itkCriterionButterflyTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_edge_butterfly.vtk 0 0.05)

itk_add_test(NAME itkCriterionLinearTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_edge_linear.vtk 1 0.05)

itk_add_test(NAME itkCriterionLoopTriangleEdgeCellSubdivisionQuadEdgeMeshFilterTest
  COMMAND ITKQuadEdgeMeshFilteringTestDriver
  itkCriterionEdgeCellSubdivisionQuadEdgeMeshFilterTest
  DATA{${INPUTDATA}/venus.vtk} ${TEMP}/venus_criterion_edge_loop.vtk 2 0.05)
