itk_module_test()
set(ITK_IO_HDF5Tests
  itkHDF5ImageIOTest.cxx
  itkHDF5ImageIOStreamingReadWriteTest.cxx
)

CreateTestDriver(ITK_IO_HDF5  "${ITK_IO_HDF5-Test_LIBRARIES}" "${ITK_IO_HDF5Tests}")

add_test(NAME itkHDF5ImageIOTest
  COMMAND ITK_IO_HDF5TestDriver itkHDF5ImageIOTest ${ITK_TEST_OUTPUT_DIR} )
add_test(NAME itkHDF5ImageIOStreamingReadWriteTest
  COMMAND ITK_IO_HDF5TestDriver itkHDF5ImageIOStreamingReadWriteTest ${ITK_TEST_OUTPUT_DIR} )
