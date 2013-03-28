itk_module_test()
set(ITKIOTransformMatlabTests
itkIOTransformMatlabTest.cxx
itkIOTransformMatlabTemplateTest.cxx
)

CreateTestDriver(ITKIOTransformMatlab "${ITKIOTransformMatlab-Test_LIBRARIES}" "${ITKIOTransformMatlabTests}")

itk_add_test(NAME itkIOTransformMatlabTest
      COMMAND ITKIOTransformMatlabTestDriver itkIOTransformMatlabTest)
itk_add_test(NAME itkIOTransformMatlabTemplateTest
      COMMAND ITKIOTransformMatlabTestDriver itkIOTransformMatlabTemplateTest)
