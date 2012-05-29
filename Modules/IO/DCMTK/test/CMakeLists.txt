itk_module_test()
set(ITKIODCMTKTests itkDCMTKImageIOTest.cxx)


# HACK BUG HACK BUG

CreateTestDriver(ITKIODCMTK "${ITKIODCMTK-Test_LIBRARIES}" "${ITKIODCMTKTests}")

itk_add_test(NAME itkDCMTKImageIOTest1
      COMMAND ITKIODCMTKTestDriver itkDCMTKImageIOTest
              DATA{${ITK_DATA_ROOT}/Input/dicom-sc_cs-1.dcm}
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKImageIOTest.dcm
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKImageIOTest.png
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKRescaleImageIOTest.dcm)
itk_add_test(NAME itkDCMTKImageIOTest3
      COMMAND ITKIODCMTKTestDriver itkDCMTKImageIOTest
              DATA{${ITK_DATA_ROOT}/Input/012345.002.050}
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKImageIOTest3.dcm
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKImageIOTest3.png
              ${ITK_TEST_OUTPUT_DIR}/itkDCMTKRescaleImageIOTest3.dcm)
