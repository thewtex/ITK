itk_module_test()
set(ITKVideoFilteringTests
  itkFrameAverageVideoFilterTest.cxx
  itkDecimateFramesVideoFilterTest.cxx
  itkImageFilterToVideoFilterWrapperTest.cxx
  itkFrameDifferenceVideoFilterTest.cxx
)

CreateTestDriver(ITKVideoFiltering "${ITKVideoFiltering-Test_LIBRARIES}" "${ITKVideoFilteringTests}")

#TODO: find a better way to locate the external data
set(DataPath "${CMAKE_BINARY_DIR}/ExternalData/Modules/Video/Filtering/test/Input")
# FrameAverageVideoFilter
itk_add_test(NAME FrameAverageVideoFilterTest
              COMMAND ITKVideoFilteringTestDriver itkFrameAverageVideoFilterTest
              # Input
              "${DataPath}/frame0.jpg,${DataPath}/frame1.jpg,${DataPath}/frame2.jpg,${DataPath}/frame3.jpg,${DataPath}/frame4.jpg"
#,DATA{Input/frame2.jpg},DATA{Input/frame3.jpg},DATA{Input/frame4.jpg}"
              # Output
              "${ITK_TEST_OUTPUT_DIR}/avg_frame0.png,${ITK_TEST_OUTPUT_DIR}/avg_frame1.png,${ITK_TEST_OUTPUT_DIR}/avg_frame2.png"
              )

# DecimateFramesVideoFilter
itk_add_test(NAME DecimateFramesVideoFilterTest
              COMMAND ITKVideoFilteringTestDriver itkDecimateFramesVideoFilterTest
              # Input
              "${DataPath}/frame0.jpg,${DataPath}/frame1.jpg,${DataPath}/frame2.jpg,${DataPath}/frame3.jpg,${DataPath}/frame4.jpg"
              # Output
              "${ITK_TEST_OUTPUT_DIR}/dec_frame0.png,${ITK_TEST_OUTPUT_DIR}/dec_frame1.png"
              )

# ImageFilterToVideoFilterWrapperTest
itk_add_test(NAME ImageFilterToVideoFilterWrapperTest
              COMMAND ITKVideoFilteringTestDriver itkImageFilterToVideoFilterWrapperTest
              # Input
              "${DataPath}/frame0.jpg,${DataPath}/frame1.jpg,${DataPath}/frame2.jpg,${DataPath}/frame3.jpg,${DataPath}/frame4.jpg"
              # Output
              "${ITK_TEST_OUTPUT_DIR}/wrapper_frame0.png,${ITK_TEST_OUTPUT_DIR}/wrapper_frame1.png,${ITK_TEST_OUTPUT_DIR}/wrapper_frame2.png,${ITK_TEST_OUTPUT_DIR}/wrapper_frame3.png,${ITK_TEST_OUTPUT_DIR}/wrapper_frame4.png"
              )

# FrameDifferenceVideoFilter
itk_add_test(NAME FrameDifferenceVideoFilterTest
              COMMAND ITKVideoFilteringTestDriver itkFrameDifferenceVideoFilterTest
              )
