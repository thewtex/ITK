IF (ITK_USE_GPU)

itk_module_test()

set(ITK-GPUCommon-tests
 itkGPUImageTest.cxx
 itkGPUImageFilterTest.cxx
 itkGPUImageFilterTestTemp.cxx )

include_directories(${OPENCL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/../)

CreateTestDriver(ITK-GPUCommon "${ITK-GPUCommon-Test_LIBRARIES}" "${ITK-GPUCommon-tests}" "${itkGPUHDR}")

add_test(NAME itkGPUImageFilterTest

        COMMAND ITK-GPUCommonTestDriver itkGPUImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${TEMP}/gpuImageFilterTest.png)

add_test(NAME itkGPUImageTest COMMAND ITK-GPUCommonTestDriver itkGPUImageTest)


#temporary test
add_test(NAME itkGPUImageFilterTestTemp
        COMMAND ITK-GPUCommonTestDriver itkGPUImageFilterTestTemp
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png




        ${TEMP}/gpuImageFilterTest.png)

ENDIF(ITK_USE_GPU)
