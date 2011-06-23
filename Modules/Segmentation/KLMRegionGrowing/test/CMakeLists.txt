itk_module_test()
set(ITK_KLMRegionGrowingTests
itkKLMRegionGrowingHeaderTest.cxx
itkRegionGrow2DTest.cxx
)

CreateTestDriver(ITK_KLMRegionGrowing  "${ITK_KLMRegionGrowing-Test_LIBRARIES}" "${ITK_KLMRegionGrowingTests}")

itk_add_test(NAME itkKLMRegionGrowingHeaderTest
      COMMAND ITK_KLMRegionGrowingTestDriver itkKLMRegionGrowingHeaderTest)
itk_add_test(NAME itkRegionGrow2DTest
      COMMAND ITK_KLMRegionGrowingTestDriver itkRegionGrow2DTest)
