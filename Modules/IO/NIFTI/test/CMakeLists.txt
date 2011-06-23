itk_module_test()
set(ITK_IO_NIFTITests
itkIONIFTIHeaderTest.cxx
itkNiftiImageIOTest.cxx
itkNiftiImageIOTest2.cxx
itkNiftiImageIOTest3.cxx
itkNiftiImageIOTest4.cxx
itkNiftiImageIOTest5.cxx
itkNiftiImageIOTest6.cxx
itkNiftiImageIOTest7.cxx
itkNiftiImageIOTest8.cxx
itkNiftiImageIOTest9.cxx
itkNiftiImageIOTest10.cxx
itkNiftiImageIOTest11.cxx
itkNiftiReadAnalyzeTest.cxx
)

add_library(ITK_IO_NIFTI-TestSupport  itkNiftiImageIOTestHelper.cxx)
itk_module_target_label(ITK_IO_NIFTI-TestSupport)
target_link_libraries(ITK_IO_NIFTI-TestSupport ${ITK_IO_NIFTI_LIBRARIES})

list(APPEND ITK_IO_NIFTI-Test_LIBRARIES ITK_IO_NIFTI-TestSupport)

CreateTestDriver(ITK_IO_NIFTI  "${ITK_IO_NIFTI-Test_LIBRARIES}" "${ITK_IO_NIFTITests}")

itk_add_test(NAME itkIONIFTIHeaderTest
      COMMAND ITK_IO_NIFTITestDriver itkIONIFTIHeaderTest)
itk_add_test(NAME itkNiftiIOBigEndianCompressed.hdr
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOBigEndianCompressed.hdr ${ITK_DATA_ROOT}/Input/BigEndianCompressed.hdr)
itk_add_test(NAME itkNiftiIOBigEndianCompressed.img
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOBigEndianCompressed.img ${ITK_DATA_ROOT}/Input/BigEndianCompressed.img)
itk_add_test(NAME itkNiftiIOBigEndianCompressed.img.gz
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOBigEndianCompressed.img.gz ${ITK_DATA_ROOT}/Input/BigEndianCompressed.img.gz)
itk_add_test(NAME itkNiftiIOBigEndian
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOBigEndian ${ITK_DATA_ROOT}/Input/BigEndian.hdr)
itk_add_test(NAME itkNiftiIOLittleEndianCompressed
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiImageIOTest ${ITK_DATA_ROOT}/Input/LittleEndianCompressed.hdr)
itk_add_test(NAME itkNiftiIOLittleEndian
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOLittleEndian ${ITK_DATA_ROOT}/Input/LittleEndian.hdr)
itk_add_test(NAME itkNiftiIOInternalTests
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkNiftiIOShouldFail
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest2
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOShouldFail false ${ITK_DATA_ROOT}/Input/ramp.gipl)
itk_add_test(NAME itkNiftiIOShouldSucceed
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest2
              ${ITK_TEST_OUTPUT_DIR} itkNiftiIOShouldSucceed true ${ITK_DATA_ROOT}/Input/LittleEndian.hdr)
itk_add_test(NAME itkDirCosinesTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest4 ${ITK_TEST_OUTPUT_DIR} )
itk_add_test(NAME itkNiftiSlopeInterceptTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest5 ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkNiftiVectorImageTest2
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest6 ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkNiftiDiffusionTensor3dImageTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest7 ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkNiftiSymmetricSecondRankTensorImageTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest8 ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkNiftiRGBImageTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest9 ${ITK_TEST_OUTPUT_DIR} RGBImage.nii.gz)
itk_add_test(NAME itkNiftiRGBAImageTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest10 ${ITK_TEST_OUTPUT_DIR} RGBAImage.nii.gz)
itk_add_test(NAME itkNiftiVectorImageTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest3 ${ITK_TEST_OUTPUT_DIR} )
itk_add_test(NAME itkNiftiDimensionLimitsTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiImageIOTest11 ${ITK_TEST_OUTPUT_DIR} SizeFailure.nii.gz )
itk_add_test(NAME itkNiftiReadAnalyzeTest
      COMMAND ITK_IO_NIFTITestDriver itkNiftiReadAnalyzeTest ${ITK_TEST_OUTPUT_DIR} )
