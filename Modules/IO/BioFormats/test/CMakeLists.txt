
set(ITK_AUTOLOAD_PATH "${CMAKE_CURRENT_BINARY_DIR}/..")
configure_file(itkBioFormatsImageIOTestConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/itkBioFormatsImageIOTestConfig.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

itk_module_test()
set(ITKIOBioFormatsTests
itkIOBioFormatsHeaderTest.cxx
itkBioFormatsImageIOTest.cxx
itkRGBBioFormatsImageIOTest.cxx
itkVectorImageBioFormatsImageIOTest.cxx
)

CreateTestDriver(ITKIOBioFormats  "${ITKIOBioFormats-Test_LIBRARIES}" "${ITKIOBioFormatsTests}")

itk_add_test(NAME itkBioFormatsImageIOTest
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/cthead1.png
                       ${ITK_TEST_OUTPUT_DIR}/itkBioFormatsImageIOTest.png
             itkBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/cthead1.png
             ${ITK_TEST_OUTPUT_DIR}/itkBioFormatsImageIOTest.png
             1  # number of streams
)
itk_add_test(NAME itkBioFormatsImageIOTestStream
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/cthead1.png
                       ${ITK_TEST_OUTPUT_DIR}/itkBioFormatsImageIOTestStream.png
             itkBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/cthead1.png
             ${ITK_TEST_OUTPUT_DIR}/itkBioFormatsImageIOTestStream.png
             5  # number of streams
)

itk_add_test(NAME itkRGBBioFormatsImageIOTest
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
                       ${ITK_TEST_OUTPUT_DIR}/itkRGBBioFormatsImageIOTest.png
             itkRGBBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
             ${ITK_TEST_OUTPUT_DIR}/itkRGBBioFormatsImageIOTest.png
             1  # number of streams
)
itk_add_test(NAME itkRGBBioFormatsImageIOTestStream
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
                       ${ITK_TEST_OUTPUT_DIR}/itkRGBBioFormatsImageIOTestStream.png
             itkRGBBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
             ${ITK_TEST_OUTPUT_DIR}/itkRGBBioFormatsImageIOTestStream.png
             3  # number of streams
)

itk_add_test(NAME itkVectorImageBioFormatsImageIOTest
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
                       ${ITK_TEST_OUTPUT_DIR}/itkVectorImageBioFormatsImageIOTest.png
             itkVectorImageBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
             ${ITK_TEST_OUTPUT_DIR}/itkVectorImageBioFormatsImageIOTest.png
             1  # number of streams
)
itk_add_test(NAME itkVectorImageBioFormatsImageIOTestStream
      COMMAND ITKIOBioFormatsTestDriver
             --compare ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
                       ${ITK_TEST_OUTPUT_DIR}/itkVectorImageBioFormatsImageIOTestStream.png
             itkVectorImageBioFormatsImageIOTest
             ${ITK_DATA_ROOT}/Input/RGBTestImage.tif
             ${ITK_TEST_OUTPUT_DIR}/itkVectorImageBioFormatsImageIOTestStream.png
             512  # number of streams
)
