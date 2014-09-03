itk_module_test()
set(ITKIOHDF5Tests
  itkHDF5ImageIOTest.cxx
  itkHDF5ImageIOStreamingReadWriteTest.cxx
)

CreateTestDriver(ITKIOHDF5  "${ITKIOHDF5-Test_LIBRARIES}" "${ITKIOHDF5Tests}")

itk_add_test(NAME itkHDF5ImageIOTest
  COMMAND ITKIOHDF5TestDriver itkHDF5ImageIOTest ${ITK_TEST_OUTPUT_DIR} )
itk_add_test(NAME itkHDF5ImageIOStreamingReadWriteTest
  COMMAND ITKIOHDF5TestDriver itkHDF5ImageIOStreamingReadWriteTest ${ITK_TEST_OUTPUT_DIR} )


itk_add_test(NAME itkHDF5ImageIOStreamingPastingCompressingTest_HDF5
  COMMAND ITKIOImageBaseTestDriver itkImageFileWriterStreamingPastingCompressingTest1
  DATA{${ITK_DATA_ROOT}/Input/cthead1.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreamingPastingCompressingTest hdf5 0 0 0 1 0 0 0 1)

itk_add_test(NAME itkHDF5ImageIODimensionsTest_HDF5
      COMMAND ITKIOImageBaseTestDriver itkImageFileReaderDimensionsTest
              DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR} hdf5)


itk_add_test(NAME itkHDF5ImageIOStreamingTest1_1
      COMMAND ITKIOImageBaseTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd}
              ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_1.hdf5
    itkImageFileWriterStreamingTest1 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_1.hdf5)
itk_add_test(NAME itkHDF5ImageIOStreamingTest1_2
      COMMAND ITKIOImageBaseTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd}
              ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_2.hdf5
    itkImageFileWriterStreamingTest1 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_2.hdf5 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolumeCompressed.mha} 0)
itk_add_test(NAME itkHDF5ImageIOStreamingTest1_3
      COMMAND ITKIOImageBaseTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd}
              ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_3.hdf5
    itkImageFileWriterStreamingTest1 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming1_3.hdf5 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} 1)
itk_add_test(NAME itkHDF5ImageIOStreamingTest2_4
      COMMAND ITKIOImageBaseTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd}
              ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming2_4.hdf5
    itkImageFileWriterStreamingTest2 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOStreaming2_4.hdf5)
itk_add_test(NAME itkHDF5ImageIOPastingTest2_5
      COMMAND ITKIOImageBaseTestDriver itkImageFileWriterPastingTest2
              DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOPastingTest2_5.hdf5)
itk_add_test(NAME itkHDF5ImageIOPastingTest2_6
      COMMAND ITKIOImageBaseTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd}
              ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOPastingTest2_6.hdf5
    itkImageFileWriterPastingTest2 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5} ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOPastingTest2_6.hdf5 DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5})
itk_add_test(NAME itkHDF5ImageIOPastingTest2_7
      COMMAND ITKIOImageBaseTestDriver itkImageFileWriterPastingTest2
              DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.hdf5}
      ${ITK_TEST_OUTPUT_DIR}/itkHDF5ImageIOPastingTest2_7.hdf5
      DATA{${ITK_DATA_ROOT}/Input/HeadMRVolumeCompressed.mha})
