itk_module_test()
set(ITKIOBioRadTests
itkIOBioRadHeaderTest.cxx
)

CreateTestDriver(ITKIOBioRad  "${ITKIOBioRad-Test_LIBRARIES}" "${ITKIOBioRadTests}")

itk_add_test(NAME itkIOBioRadHeaderTest
      COMMAND ITKIOBioRadTestDriver itkIOBioRadHeaderTest)
