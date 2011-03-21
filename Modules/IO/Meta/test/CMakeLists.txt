itk_module_test()
set(ITK-IO-MetaTests
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
)

CreateTestDriver(ITK-IO-Meta  "${ITK-IO-Meta-Test_LIBRARIES}" "${ITK-IO-MetaTests}")

add_test(NAME itkIOMetaHeaderTest
      COMMAND ITK-IO-MetaTestDriver itkIOMetaHeaderTest)
add_test(NAME itkMetaImageIOMetaDataTest
      COMMAND ITK-IO-MetaTestDriver itkMetaImageIOMetaDataTest
              ${ITK_TEST_OUTPUT_DIR}/MetaImageIOMetaDataTest.mhd)
add_test(NAME itkMetaImageIOGzTest
      COMMAND ITK-IO-MetaTestDriver itkMetaImageIOGzTest
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME itkMetaImageIOTest
      COMMAND ITK-IO-MetaTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/IO/HeadMRVolume.mhd
              ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mhd
    itkMetaImageIOTest ${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mhd)
add_test(NAME itkMetaImageIOShouldFailTest
      COMMAND ITK-IO-MetaTestDriver itkMetaImageIOTest
              ${ITK_DATA_ROOT}/Input/MetaImageError.mhd 1)
add_test(NAME testMetaGroup
      COMMAND ITK-IO-MetaTestDriver testMetaGroup
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaImage
      COMMAND ITK-IO-MetaTestDriver testMetaImage)
add_test(NAME testMetaLandmark
      COMMAND ITK-IO-MetaTestDriver testMetaLandmark
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaLine
      COMMAND ITK-IO-MetaTestDriver testMetaLine
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaMesh
      COMMAND ITK-IO-MetaTestDriver testMetaMesh
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaObject
      COMMAND ITK-IO-MetaTestDriver testMetaObject
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaScene
      COMMAND ITK-IO-MetaTestDriver testMetaScene
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaSurface
      COMMAND ITK-IO-MetaTestDriver testMetaSurface
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaTube
      COMMAND ITK-IO-MetaTestDriver testMetaTube
              ${ITK_TEST_OUTPUT_DIR})
add_test(NAME testMetaUtils
      COMMAND ITK-IO-MetaTestDriver testMetaUtils
              ${ITK_TEST_OUTPUT_DIR})
