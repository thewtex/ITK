itk_module_test()
set(VideoIOVXLTests
  itkVXLVideoIOTest.cxx
  itkVXLVideoIOFactoryTest.cxx
  vidl_itk_istreamTest.cxx
)

# include VXL headers
find_package ( VXL REQUIRED )
include( ${VXL_CMAKE_DIR}/UseVXL.cmake )

CreateTestDriver(VideoIOVXL "${VideoIOVXL-Test_LIBRARIES}" "${VideoIOVXLTests}")

# VXLVideoIO:
add_video_test(NAME VXLVideoIOTest
               COMMAND VideoIOVXLTestDriver itkVXLVideoIOTest
               # Video Input:
               MIDAS{inde-circulation.avi.md5}
               # NonVideo Input:
               ${ITKVideo_Data_DIR}/Input/TestParameters.txt
               # Output:
               ${ITK_TEST_OUTPUT_DIR}/VXLVideoIOTest_Out.avi
               # Webcam Output:
               ${ITK_TEST_OUTPUT_DIR}/VXLVideoIOTest_CameraOut.mha
               # Width Height NumFrames FpS
               320 240 1396 14.985
               )

# VXLVideoIOFactory:
add_video_test(NAME VXLVideoIOFactoryTest
               COMMAND VideoIOVXLTestDriver itkVXLVideoIOFactoryTest
               # Video Input:
               MIDAS{inde-circulation.avi.md5}
               # Output:
               ${ITK_TEST_OUTPUT_DIR}/VXLVideoIOTest_Out.avi
               # Webcam Number:
               0
               )

# vidl_itk_istream:
add_video_test(NAME vidl_itk_istreamTest
               COMMAND VideoIOVXLTestDriver vidl_itk_istreamTest
               # Video Input:
               MIDAS{inde-circulation_short.avi.md5}
               # Output:
               ${ITK_TEST_OUTPUT_DIR}/vidl_itk_istreamTest_Out.avi
               # Width Height
               320 240
               )
