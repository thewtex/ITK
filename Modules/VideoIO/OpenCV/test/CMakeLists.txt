itk_module_test()
set(VideoIOOpenCVTests
  itkOpenCVVideoIOTest.cxx
  itkOpenCVVideoIOFactoryTest.cxx
)
find_package ( OpenCV REQUIRED )
CreateTestDriver(VideoIOOpenCV  "${VideoIOOpenCV-Test_LIBRARIES}" "${VideoIOOpenCVTests}")

# OpenCVVideoIO:
add_video_test(NAME OpenCVVideoIOTest
               COMMAND VideoIOOpenCVTestDriver itkOpenCVVideoIOTest
               # Video Input:
               MIDAS{inde-circulation.avi.md5}
               # NonVideo Input:
               ${ITKVideo_Data_DIR}/Input/TestParameters.txt
               # Output:
               ${ITK_TEST_OUTPUT_DIR}/OpenCVVideoIOTest_Out.avi
               # Webcam Output:
               ${ITK_TEST_OUTPUT_DIR}/OpenCVVideoIOTest_CameraOut.mha
               # Width Height NumFrames FpS
               320 240 1396 14.985
               )

# OpenCVVideoIOFactory:
add_video_test(NAME OpenCVVideoIOFactoryTest
               COMMAND VideoIOOpenCVTestDriver itkOpenCVVideoIOFactoryTest
               # Video Input:
               MIDAS{inde-circulation.avi.md5}
               # Output:
               ${ITK_TEST_OUTPUT_DIR}/OpenCVVideoIOTest_Out.avi
               # Webcam Number:
               0
               )
