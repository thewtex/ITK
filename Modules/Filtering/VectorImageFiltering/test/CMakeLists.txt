itk_module_test()
set(ITK-VectorImageFilteringTests
itkChannelByChannelVectorImageFilterTest.cxx
)

CreateTestDriver(ITK-VectorImageFiltering  "${ITK-VectorImageFiltering-Test_LIBRARIES}" "${ITK-VectorImageFilteringTests}")

add_test(NAME itkChannelByChannelVectorImageFilterTest
      COMMAND ITK-VectorImageFilteringTestDriver itkChannelByChannelVectorImageFilterTest)
