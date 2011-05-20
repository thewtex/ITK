itk_module(ITK-ImageStatistics DEPENDS ITK-ImageFilterBase ITK-Statistics TEST_DEPENDS ITK-ImageIntensity ITK-TestKernel ITK-Thresholding ITK-ImageLabel ITK-SpatialObjects)
#extra test dependency on Thresholding is introduced by itkNormalizedCorrelationImageFilterTest;
#extra test dependency on ImageLabel is introduced by tkBinaryProjectionImageFilterTest.
#extra test dependency on SpatialObjects is introduced by itkImageMomentsTest
