itk_module_test()
set(ITKFiniteDifferenceTests
itkFiniteDifferenceHeaderTest.cxx
)

CreateTestDriver(ITKFiniteDifference "${ITKFiniteDifference_LIBRARIES}"
  "${ITKFiniteDifferenceTests}")

itk_add_test(NAME itkFiniteDifferenceHeaderTest
  COMMAND ITKFiniteDifferenceTestDriver itkFiniteDifferenceHeaderTest)
