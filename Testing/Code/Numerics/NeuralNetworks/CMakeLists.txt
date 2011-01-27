include_regular_expression("^.*$")

if( NOT ITK_DISABLE_CXX_TESTING )
 if(ITK_BINARY_DIR)
  if(TCL_TCLSH)
    add_test(PrintSelf-NeuralNets ${TCL_TCLSH}
         ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl
             ${ITK_SOURCE_DIR}/Code/Numerics/NeuralNetworks)
  endif(TCL_TCLSH)
endif(ITK_BINARY_DIR)

set(KIT NeuralNetworks)
set(KIT_LIBS
  ITKIO ITKNumerics ITKStatistics
)

set(CXX_TESTS ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)

set(KitTests
  NNetClassifierTest1
  NNetClassifierTest2
  NNetClassifierTest3
  NNetClassifierTest4
  QPropXORTest1
  RBFTest1
  XORTest1
  XORTest2
  itkNeuralNetworksPrintTest
)

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkv4TestMainAfter.inc\"")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KitTests}
  EXTRA_INCLUDE itkv4TestMain.h
  FUNCTION ProcessArguments
  )

add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT_LIBS})

set(NEURAL_NETWORKS_HEADER_TEST  ${CXX_TEST_PATH}/itkNeuralNetworksHeaderTest)
add_executable(itkNeuralNetworksHeaderTest itkNeuralNetworksHeaderTest.cxx)
target_link_libraries(itkNeuralNetworksHeaderTest ${KIT_LIBS})
add_test(itkNeuralNetworksHeaderTest ${NEURAL_NETWORKS_HEADER_TEST})

add_test(QPropXORTest1 ${CXX_TESTS}
         QPropXORTest1 ${ITK_DATA_ROOT}/Input/NeuralNetworks/qpropxortest.txt)
add_test(XORTest1 ${CXX_TESTS}
         XORTest1 ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
add_test(XORTest2 ${CXX_TESTS}
         XORTest2 ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
add_test(NNetClassifierTest1 ${CXX_TESTS}
         NNetClassifierTest1 ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt
                             ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
add_test(NNetClassifierTest2 ${CXX_TESTS}
         NNetClassifierTest2 ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt
                             ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
add_test(NNetClassifierTest3 ${CXX_TESTS}
         NNetClassifierTest3 ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt
                             ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
add_test(NNetClassifierTest4 ${CXX_TESTS}
         NNetClassifierTest4 ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt
                             ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
add_test(RBFTest1 ${CXX_TESTS}
         RBFTest1 ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_train.txt
                  ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_test.txt)

add_test( itkNeuralNetworksPrintTest ${CXX_TESTS}
          itkNeuralNetworksPrintTest)

if(ITK_OBJCXX_COMPILER_WORKS)
  set(NEURALNETWORKS_OBJCXX_TEST ${CXX_TEST_PATH}/itkNeuralNetworksHeaderObjCxxTest)
  add_test(itkNeuralNetworksHeaderObjCxxTest ${NEURALNETWORKS_OBJCXX_TEST})
  add_executable(itkNeuralNetworksHeaderObjCxxTest itkNeuralNetworksHeaderObjCxxTest.mm)
  target_link_libraries(itkNeuralNetworksHeaderObjCxxTest ITKIO ITKNumerics ITKStatistics)
endif(ITK_OBJCXX_COMPILER_WORKS)

endif( NOT ITK_DISABLE_CXX_TESTING )
