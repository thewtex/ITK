itk_module_test()
set(ITK_NeuralNetworksTests
itkNeuralNetworksHeaderTest.cxx
NNetClassifierTest1.cxx
NNetClassifierTest2.cxx
NNetClassifierTest3.cxx
NNetClassifierTest4.cxx
RBFTest1.cxx
QPropXORTest1.cxx
XORTest1.cxx
XORTest2.cxx
)

CreateTestDriver(ITK_NeuralNetworks  "${ITK_NeuralNetworks-Test_LIBRARIES}" "${ITK_NeuralNetworksTests}")

itk_add_test(NAME itkNeuralNetworksHeaderTest
      COMMAND ITK_NeuralNetworksTestDriver itkNeuralNetworksHeaderTest)
itk_add_test(NAME NNetClassifierTest1
      COMMAND ITK_NeuralNetworksTestDriver NNetClassifierTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
itk_add_test(NAME NNetClassifierTest2
      COMMAND ITK_NeuralNetworksTestDriver NNetClassifierTest2
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
itk_add_test(NAME NNetClassifierTest3
      COMMAND ITK_NeuralNetworksTestDriver NNetClassifierTest3
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
itk_add_test(NAME NNetClassifierTest4
      COMMAND ITK_NeuralNetworksTestDriver NNetClassifierTest4
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/test.txt)
itk_add_test(NAME RBFTest1
      COMMAND ITK_NeuralNetworksTestDriver RBFTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_train.txt ${ITK_DATA_ROOT}/Input/NeuralNetworks/rbf_multivar_gauss_test.txt)
itk_add_test(NAME QPropXORTest1
      COMMAND ITK_NeuralNetworksTestDriver QPropXORTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/qpropxortest.txt)
itk_add_test(NAME XORTest1
      COMMAND ITK_NeuralNetworksTestDriver XORTest1
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
itk_add_test(NAME XORTest2
      COMMAND ITK_NeuralNetworksTestDriver XORTest2
              ${ITK_DATA_ROOT}/Input/NeuralNetworks/xortest.txt)
