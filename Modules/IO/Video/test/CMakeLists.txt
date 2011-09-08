itk_module_test()
set(ITKIOVideoTests
  itkVideoFileReaderWriterTest.cxx
  itkFileListVideoIOTest.cxx
  itkFileListVideoIOFactoryTest.cxx
)

# For adding midas test with "Video" label
include(${ITKIOVideo_SOURCE_DIR}/add_video_test.cmake)
CreateTestDriver(ITKIOVideo  "${ITKIOVideo-Test_LIBRARIES}" "${ITKIOVideoTests}")

# VideoFileRederWriterTest:
add_video_test(NAME VideoFileReaderWriterTest
               COMMAND ITKIOVideoTestDriver itkVideoFileReaderWriterTest
               # Input
               "MIDAS{frame0.jpg.md5},MIDAS{frame1.jpg.md5},MIDAS{frame2.jpg.md5},MIDAS{frame3.jpg.md5},MIDAS{frame4.jpg.md5}"
               # Output
               "${ITK_TEST_OUTPUT_DIR}/frame0.png,${ITK_TEST_OUTPUT_DIR}/frame1.png,${ITK_TEST_OUTPUT_DIR}/frame2.png,${ITK_TEST_OUTPUT_DIR}/frame3.png,${ITK_TEST_OUTPUT_DIR}/frame4.png"
               )


# FileListVideoIO:
add_video_test(NAME FileListVideoIOTest
               COMMAND ITKIOVideoTestDriver itkFileListVideoIOTest
               # Video Input:
               "MIDAS{frame0.jpg.md5},MIDAS{frame1.jpg.md5},MIDAS{frame2.jpg.md5},MIDAS{frame3.jpg.md5},MIDAS{frame4.jpg.md5}"
               # NonVideo Input:
               ${ITKVideo_Data_DIR}/Input/TestParameters.txt
               # Output:
               "${ITK_TEST_OUTPUT_DIR}/frame0.png,${ITK_TEST_OUTPUT_DIR}/frame1.png,${ITK_TEST_OUTPUT_DIR}/frame2.png,${ITK_TEST_OUTPUT_DIR}/frame3.png,${ITK_TEST_OUTPUT_DIR}/frame4.png"
               # Webcam Output:
               ${ITK_TEST_OUTPUT_DIR}/FileListVideoIOTest_CameraOut.mha
               # Height Width NumFrames FpS
               320 240 5 1
               )

# FileListVideoIOFactory:
add_video_test(NAME FileListVideoIOFactoryTest
               COMMAND ITKIOVideoTestDriver itkFileListVideoIOFactoryTest
               # Video Input:
               "MIDAS{frame0.jpg.md5},MIDAS{frame1.jpg.md5},MIDAS{frame2.jpg.md5},MIDAS{frame3.jpg.md5},MIDAS{frame4.jpg.md5}"
               # Output:
               "${ITK_TEST_OUTPUT_DIR}/frame0.png,${ITK_TEST_OUTPUT_DIR}/frame1.png,${ITK_TEST_OUTPUT_DIR}/frame2.png,${ITK_TEST_OUTPUT_DIR}/frame3.png,${ITK_TEST_OUTPUT_DIR}/frame4.png"
               # Webcam Number:
               0
               )
