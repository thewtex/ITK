itk_module_test()
set(ITK_IO_VTKTests
itkVTKImageIO2Test.cxx
itkVTKImageIOTest.cxx
itkIOVTKHeaderTest.cxx
itkVTKImageIO2Test2.cxx
itkVTKImageIOTest2.cxx
)

CreateTestDriver(ITK_IO_VTK  "${ITK_IO_VTK-Test_LIBRARIES}" "${ITK_IO_VTKTests}")

itk_add_test(NAME itkIOVTKHeaderTest
      COMMAND ITK_IO_VTKTestDriver itkIOVTKHeaderTest)
itk_add_test(NAME itkVTKImageIO2Test
      COMMAND ITK_IO_VTKTestDriver itkVTKImageIO2Test
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkVTKImageIOTest
      COMMAND ITK_IO_VTKTestDriver itkVTKImageIOTest
              ${ITK_TEST_OUTPUT_DIR}/test1.vtk ${ITK_TEST_OUTPUT_DIR}/test2.vtk)
itk_add_test(NAME itkVTKImageIO2Test2
      COMMAND ITK_IO_VTKTestDriver itkVTKImageIO2Test2
              ${ITK_TEST_OUTPUT_DIR}/itkVTKImageIO2Test2.vtk)
itk_add_test(NAME itkVTKImageIOTest2
      COMMAND ITK_IO_VTKTestDriver itkVTKImageIOTest2
              ${ITK_DATA_ROOT}/Input/VHFColor.mhd ${ITK_TEST_OUTPUT_DIR}/testVector.vtk)
