itk_module_test()
set(VideoCoreOpenCVTests
  itkOpenCVVideoCaptureTest.cxx
  itkOpenCVImageBridgeTest.cxx
)
find_package ( OpenCV REQUIRED )
CreateTestDriver(VideoCoreOpenCV  "${VideoCoreOpenCV-Test_LIBRARIES}" "${VideoCoreOpenCVTests}")

include_directories(${VideoIOOpenCV_INCLUDE_DIRS})

# OpenCVVideoCapture:
add_video_test(NAME OpenCVVideoCaptureTest
               COMMAND VideoCoreOpenCVTestDriver itkOpenCVVideoCaptureTest
               # Video Input:
               MIDAS{inde-circulation.avi.md5}
               # Scalar Output:
               ${ITK_TEST_OUTPUT_DIR}/OpenCVVideoCaptureTest_ScalarOut.avi
               # RGB Output:
               ${ITK_TEST_OUTPUT_DIR}/OpenCVVideoCaptureTest_RGBOut.avi
               # Width Height
               320 240
               )

# OpenCVImageBridge:
#   NOTE: For some reason, ITK reads jpg files slightly differently than
#   OpenCV, so we need both jpg and mha versions to guanrantee consistency
add_video_test(NAME OpenCVImageBridgeTest
               COMMAND VideoCoreOpenCVTestDriver itkOpenCVImageBridgeTest
               # Input Grayscale Image jpg
               MIDAS{frame0_gray.jpg.md5}
               # Input RGB Image jpg:
               MIDAS{frame0.jpg.md5}
               # Input RGB Image mha:
               MIDAS{frame0.mha.md5}
               )
