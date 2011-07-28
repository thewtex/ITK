itk_module_test()
set(itkTestKernelTests
itkTestKernelHeaderTest.cxx
)

CreateTestDriver(ITKTestKernel "${ITKTestKernel-Test_LIBRARIES}"
  "${itkTestKernelTests}")

itk_add_test(NAME itkTestKernelHeaderTest
      COMMAND ITKTestKernelTestDriver itkTestKernelHeaderTest)
