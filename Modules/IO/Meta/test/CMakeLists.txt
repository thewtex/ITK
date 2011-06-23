itk_module_test()
set(ITK_IO_MetaTests
itkMetaImageIOMetaDataTest.cxx
itkIOMetaHeaderTest.cxx
itkMetaImageIOGzTest.cxx
itkMetaImageIOTest.cxx
testMetaArray.cxx
testMetaBlob.cxx
testMetaCommand.cxx
testMetaGroup.cxx
testMetaImage.cxx
testMetaLandmark.cxx
testMetaLine.cxx
testMetaMesh.cxx
testMetaObject.cxx
testMetaScene.cxx
testMetaSurface.cxx
testMetaTube.cxx
testMetaUtils.cxx
itkMetaImageStreamingIOTest.cxx
itkMetaImageStreamingWriterIOTest.cxx
)

CreateTestDriver(ITK_IO_Meta  "${ITK_IO_Meta-Test_LIBRARIES}" "${ITK_IO_MetaTests}")

itk_add_test(NAME itkIOMetaHeaderTest
      COMMAND ITK_IO_MetaTestDriver itkIOMetaHeaderTest)
itk_add_test(NAME itkMetaImageIOMetaDataTest
      COMMAND ITK_IO_MetaTestDriver itkMetaImageIOMetaDataTest
              ${ITK_TEST_OUTPUT_DIR}/MetaImageIOMetaDataTest.mhd)
itk_add_test(NAME itkMetaImageIOGzTest
      COMMAND ITK_IO_MetaTestDriver itkMetaImageIOGzTest
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkMetaImageIOTest
      COMMAND ITK_IO_MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd
              ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mhd
    itkMetaImageIOTest ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mhd)
itk_add_test(NAME itkMetaImageIOShouldFailTest
      COMMAND ITK_IO_MetaTestDriver itkMetaImageIOTest
              ${ITK_DATA_ROOT}/Input/MetaImageError.mhd 1)
itk_add_test(NAME testMetaGroup
      COMMAND ITK_IO_MetaTestDriver testMetaGroup
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaImage
      COMMAND ITK_IO_MetaTestDriver testMetaImage)
itk_add_test(NAME testMetaLandmark
      COMMAND ITK_IO_MetaTestDriver testMetaLandmark
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaLine
      COMMAND ITK_IO_MetaTestDriver testMetaLine
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaMesh
      COMMAND ITK_IO_MetaTestDriver testMetaMesh
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaObject
      COMMAND ITK_IO_MetaTestDriver testMetaObject
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaScene
      COMMAND ITK_IO_MetaTestDriver testMetaScene
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaSurface
      COMMAND ITK_IO_MetaTestDriver testMetaSurface
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaTube
      COMMAND ITK_IO_MetaTestDriver testMetaTube
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaUtils
      COMMAND ITK_IO_MetaTestDriver testMetaUtils
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME MetaCommandRequiredOptionsWithMultipleStrings
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 7 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7)
itk_add_test(NAME MetaCommandRequiredOptionsWithMultipleStringsAtEnd
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 7 -r2_ri 7 -r1c_rs StringValue1 StringValue2 StringValue3)
itk_add_test(NAME MetaCommandOptionalWithRequired
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 107 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_ri 1 -o2_ri 99)
itk_add_test(NAME MetaCommandOptionalOptoinsWithOutArguments
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 207 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_oi -o2_oi)
itk_add_test(NAME MetaCommandOptionalOptoinsWithOutArguments1
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 107 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_oi 1 -o2_oi 99)
itk_add_test(NAME MetaCommandRequiredAndOptionalOptoinsWithArguments
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 207 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_ri 1 -o2_ri 99 -o3_ri_oi 50 50)
itk_add_test(NAME MetaCommandRequiredAndOptionalOptoinsWithOutArguments
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 257 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_ri 1 -o2_ri 99 -o3_ri_oi 50)
itk_add_test(NAME MetaCommandRequiredAndOptionalOptoinsWithOutArguments-1
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 0 -sum 457 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o1_ri 1 -o2_ri 99 -o3_ri_oi 50 -o1_oi -o2_oi)
itk_add_test(NAME MetaCommandRequiredOptionsWithMissingMultipleStrings
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 1 -sum 7 -r1c_rs -r2_ri 7)
itk_add_test(NAME MetaCommandRequiredOptionsWithMissingMultipleStringsAtEnd
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 1 -sum 7 -r2_ri 7 -r1c_rs)
itk_add_test(NAME MetaCommandOptionalWithRequiredMissing
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 1 -sum 107 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri -o1_ri 1 -o2_ri 99)
itk_add_test(NAME MetaCommandRequiredAndOptionalOptoinsWithArgumentsMissing
      COMMAND ITK_IO_MetaTestDriver testMetaCommand
              -ExpectedFailStatus 1 -sum 207 -r1c_rs StringValue1 StringValue2 StringValue3 -r2_ri 7 -o3_ri_oi)
itk_add_test(NAME testMetaArray
      COMMAND ITK_IO_MetaTestDriver testMetaArray
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME testMetaBlob
      COMMAND ITK_IO_MetaTestDriver testMetaBlob
              ${ITK_TEST_OUTPUT_DIR})
itk_add_test(NAME itkMetaImageStreamingIOTest
      COMMAND ITK_IO_MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd
              ${ITK_TEST_OUTPUT_DIR}/HeadMRVolumeStreamed.mhd
    itkMetaImageStreamingIOTest ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/HeadMRVolumeStreamed.mhd)
itk_add_test(NAME itkMetaImageCompressedStreamingIOTest
      COMMAND ITK_IO_MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd
              ${ITK_TEST_OUTPUT_DIR}/HeadMRVolumeCompressedStreamed.mha
    itkMetaImageStreamingIOTest ${ITK_DATA_ROOT}/Input/HeadMRVolumeCompressed.mha ${ITK_TEST_OUTPUT_DIR}/HeadMRVolumeCompressedStreamed.mha)
itk_add_test(NAME itkMetaImageStreamingWriterIOTest
      COMMAND ITK_IO_MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd
              ${ITK_TEST_OUTPUT_DIR}/MetaImageStreamingWriterIOTest.mha
    itkMetaImageStreamingWriterIOTest ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/MetaImageStreamingWriterIOTest.mha)
itk_add_test(NAME itkMetaImageStreamingWriterIOTest2
      COMMAND ITK_IO_MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Input/mri3D.mhd
              ${ITK_TEST_OUTPUT_DIR}/mri3DWriteStreamed.mha
    itkMetaImageStreamingWriterIOTest ${ITK_DATA_ROOT}/Input/mri3D.mhd ${ITK_TEST_OUTPUT_DIR}/mri3DWriteStreamed.mha)
