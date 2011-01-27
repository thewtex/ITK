add_subdirectory(Statistics)

add_subdirectory(FEM)
add_subdirectory(NeuralNetworks)

if( NOT ITK_DISABLE_CXX_TESTING )
if(ITK_BINARY_DIR)
  if(TCL_TCLSH)
    add_test(PrintSelf-Numerics ${TCL_TCLSH}
             ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl
             ${ITK_SOURCE_DIR}/Code/Numerics)
  endif(TCL_TCLSH)
endif(ITK_BINARY_DIR)

set(KIT Numerics)
set(CXX_TESTS ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)

set(KIT_LIBS ITKNumerics ITKIO ITKStatistics)
set(KitTests
itkAmoebaOptimizerTest.cxx
itkCompositeValleyFunctionTest.cxx
itkConjugateGradientOptimizerTest.cxx
itkCumulativeGaussianOptimizerTest.cxx
itkExhaustiveOptimizerTest.cxx
itkFRPROptimizerTest.cxx
itkGradientDescentOptimizerTest.cxx
itkLBFGSBOptimizerTest.cxx
itkLBFGSOptimizerTest.cxx
itkLevenbergMarquardtOptimizerTest.cxx
itkMultivariateLegendrePolynomialTest.cxx
itkNumericsPrintTest.cxx
itkNumericsTest.cxx
itkOptimizersHierarchyTest.cxx
itkPowellOptimizerTest.cxx
itkRegularStepGradientDescentOptimizerTest.cxx
itkSPSAOptimizerTest.cxx
itkSymmetricEigenSystemTest.cxx
itkVersorRigid3DTransformOptimizerTest.cxx
itkVersorTransformOptimizerTest.cxx
)

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkv4TestMainAfter.inc\"")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KitTests}
  EXTRA_INCLUDE itkv4TestMain.h
  FUNCTION ProcessArguments
  )

add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT_LIBS})

set(NUMERICS_HEADER_TEST ${CXX_TEST_PATH}/itkNumericsHeaderTest)
add_executable(itkNumericsHeaderTest itkNumericsHeaderTest.cxx)
target_link_libraries(itkNumericsHeaderTest ITKNumerics ITKStatistics)
add_test(itkNumericsHeaderTest ${NUMERICS_HEADER_TEST})

add_test(itkAmoebaOptimizerTest ${CXX_TESTS} itkAmoebaOptimizerTest)
add_test(itkCompositeValleyFunctionTest ${CXX_TESTS} itkCompositeValleyFunctionTest)
add_test(itkConjugateGradientOptimizerTest ${CXX_TESTS} itkConjugateGradientOptimizerTest)
add_test(itkCumulativeGaussianOptimizerTest ${CXX_TESTS} itkCumulativeGaussianOptimizerTest)
add_test(itkExhaustiveOptimizerTest ${CXX_TESTS} itkExhaustiveOptimizerTest)
add_test(itkFRPROptimizerTest ${CXX_TESTS} itkFRPROptimizerTest)
add_test(itkGradientDescentOptimizerTest ${CXX_TESTS} itkGradientDescentOptimizerTest)
add_test(itkLBFGSBOptimizerTest ${CXX_TESTS} itkLBFGSBOptimizerTest)
add_test(itkLBFGSOptimizerTest ${CXX_TESTS} itkLBFGSOptimizerTest)
add_test(itkLevenbergMarquardtOptimizerTest ${CXX_TESTS} itkLevenbergMarquardtOptimizerTest)
add_test(itkMultivariateLegendrePolynomialTest ${CXX_TESTS} itkMultivariateLegendrePolynomialTest)
add_test(itkNumericsPrintTest ${CXX_TESTS} itkNumericsPrintTest)
add_test(itkNumericsTest ${CXX_TESTS} itkNumericsTest)
add_test(itkOptimizersHierarchyTest ${CXX_TESTS} itkOptimizersHierarchyTest)
add_test(itkPowellOptimizerTest ${CXX_TESTS} itkPowellOptimizerTest)
add_test(itkRegularStepGradientDescentOptimizerTest ${CXX_TESTS} itkRegularStepGradientDescentOptimizerTest)
add_test(itkSymmetricEigenSystemTest ${CXX_TESTS} itkSymmetricEigenSystemTest)
add_test(itkSPSAOptimizerTest ${CXX_TESTS} itkSPSAOptimizerTest)
add_test(itkVersorRigid3DTransformOptimizerTest ${CXX_TESTS} itkVersorRigid3DTransformOptimizerTest)
add_test(itkVersorTransformOptimizerTest ${CXX_TESTS} itkVersorTransformOptimizerTest)

if(ITK_OBJCXX_COMPILER_WORKS)
  set(NUMERICS_OBJCXX_TEST ${CXX_TEST_PATH}/itkNumericsHeaderObjCxxTest)
  add_test(itkNumericsHeaderObjCxxTest ${NUMERICS_OBJCXX_TEST})
  add_executable(itkNumericsHeaderObjCxxTest itkNumericsHeaderObjCxxTest.mm)
  target_link_libraries(itkNumericsHeaderObjCxxTest ITKNumerics ITKStatistics)
endif(ITK_OBJCXX_COMPILER_WORKS)

endif( NOT ITK_DISABLE_CXX_TESTING )


