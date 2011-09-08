itk_module_test()
set(ITKVideoCoreTests
  itkRingBufferTest.cxx
  itkTemporalRegionTest.cxx
  itkTemporalDataObjectTest.cxx
  itkTemporalProcessObjectTest.cxx
  itkVideoStreamTest.cxx
  itkVideoSourceTest.cxx
  itkVideoToVideoFilterTest.cxx
)


CreateTestDriver(ITKVideoCore  "${ITKVideoCore-Test_LIBRARIES}" "${ITKVideoCoreTests}" )

# VideoToVideoFilterTest
add_video_test( NAME VideoToVideoFilterTest
                COMMAND ITKVideoCoreTestDriver itkVideoToVideoFilterTest )

# VideoSourceTest
add_video_test( NAME VideoSourceTest
                COMMAND ITKVideoCoreTestDriver itkVideoSourceTest )

# VideoStreamTest
add_video_test( NAME VideoStreamTest
                COMMAND ITKVideoCoreTestDriver itkVideoStreamTest )

# TemporalProcessObjectTest
add_video_test( NAME TemporalProcessObjectTest
                COMMAND ITKVideoCoreTestDriver itkTemporalProcessObjectTest )

# RingBufferTest
add_video_test( NAME RingBufferTest
                COMMAND ITKVideoCoreTestDriver itkRingBufferTest )

# Testing Temporal Regions
add_video_test( NAME TemporalRegionTest
                COMMAND ITKVideoCoreTestDriver itkTemporalRegionTest )

# Testing Temporal Data Object
add_video_test( NAME TemporalDataObjectTest
                COMMAND ITKVideoCoreTestDriver itkTemporalDataObjectTest )
