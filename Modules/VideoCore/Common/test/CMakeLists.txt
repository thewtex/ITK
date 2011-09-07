itk_module_test()
set(VideoCoreCommonTests
  itkRingBufferTest.cxx
  itkTemporalRegionTest.cxx
  itkTemporalDataObjectTest.cxx
  itkTemporalProcessObjectTest.cxx
  itkVideoStreamTest.cxx
  itkVideoSourceTest.cxx
  itkVideoToVideoFilterTest.cxx
)


# For adding midas test with "Video" label
include(${VideoCoreCommon_SOURCE_DIR}/add_video_test.cmake)
CreateTestDriver(VideoCoreCommon  "${VideoCoreCommon-Test_LIBRARIES}" "${VideoCoreCommonTests}" )

# VideoToVideoFilterTest
add_video_test( NAME VideoToVideoFilterTest
                COMMAND VideoCoreCommonTestDriver itkVideoToVideoFilterTest )

# VideoSourceTest
add_video_test( NAME VideoSourceTest
                COMMAND VideoCoreCommonTestDriver itkVideoSourceTest )

# VideoStreamTest
add_video_test( NAME VideoStreamTest
                COMMAND VideoCoreCommonTestDriver itkVideoStreamTest )

# TemporalProcessObjectTest
add_video_test( NAME TemporalProcessObjectTest
                COMMAND VideoCoreCommonTestDriver itkTemporalProcessObjectTest )

# RingBufferTest
add_video_test( NAME RingBufferTest
                COMMAND VideoCoreCommonTestDriver itkRingBufferTest )

# Testing Temporal Regions
add_video_test( NAME TemporalRegionTest
                COMMAND VideoCoreCommonTestDriver itkTemporalRegionTest )

# Testing Temporal Data Object
add_video_test( NAME TemporalDataObjectTest
                COMMAND VideoCoreCommonTestDriver itkTemporalDataObjectTest )
