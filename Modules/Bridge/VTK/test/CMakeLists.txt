itk_module_test()
set(ITKVTKTests
itkVTKHeaderTest.cxx
)

CreateTestDriver(ITKVTK  "${ITKVTK-Test_LIBRARIES}" "${ITKVTKTests}")

itk_add_test(NAME itkVTKHeaderTest
      COMMAND ITKVTKTestDriver itkIOBioRadHeaderTest)
