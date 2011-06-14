IF(ITK_USE_GPU)

# With MS compilers on Win64, we need the /bigobj switch, else generated
# code results in objects with number of sections exceeding object file
# format.
# see http://msdn.microsoft.com/en-us/library/ms173499.aspx
if(CMAKE_CL_64 OR CMAKE_COMPILER_2005)
add_definitions(/bigobj)
endif()

itk_module_test()
set(ITKGPUCommonTests
itkGPUImageTest.cxx
itkGPUImageFilterTest.cxx
itkGPUMeanImageFilterTest.cxx
itkGPUBinaryThresholdImageFilterTest.cxx
itkGPUNeighborhoodOperatorImageFilterTest.cxx
itkGPUDiscreteGaussianImageFilterTest.cxx
itkGPUGradientAnisotropicDiffusionImageFilterTest.cxx
itkGPUDemonsRegistrationFilterTest.cxx
)

include_directories(${OPENCL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/../)

CreateTestDriver(ITKGPUCommon "${ITKGPUCommon-Test_LIBRARIES}" "${ITKGPUCommonTests}")


itk_add_test(NAME itkGPUImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUImageFilterTest
        ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
       ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png)

itk_add_test(NAME itkGPUMeanImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUMeanImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuMeanImageFilterTest.png)

itk_add_test(NAME itkGPUBinaryThresholdImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUBinaryThresholdImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuBinaryThresholdImageFilterTest.png)

itk_add_test(NAME itkGPUNeighborhoodOperatorImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUNeighborhoodOperatorImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest.png)

itk_add_test(NAME itkGPUDiscreteGaussianImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUDiscreteGaussianImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest.png)

itk_add_test(NAME itkGPUGradientAnisotropicDiffusionImageFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUGradientAnisotropicDiffusionImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuGradientAnisotropicDiffusionImageFilterTest.png)

itk_add_test(NAME itkGPUDemonsRegistrationFilterTest
        COMMAND ITKGPUCommonTestDriver itkGPUDemonsRegistrationFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${ITK_TEST_OUTPUT_DIR}/gpuImageFilterTest.png
        ${ITK_TEST_OUTPUT_DIR}/output)


ENDIF(ITK_USE_GPU)
