IF (ITK_USE_GPU)

<<<<<<< HEAD
add_executable(ITK-GPUCommon-tests  ${Tests} )
itk_module_target_label(ITK-GPUCommon-tests)
target_link_libraries(ITK-GPUCommon-tests  ${ITK-GPUCommon_LIBRARIES} )

itk_add_test(NAME itkGPUImageFilterTest
        COMMAND ITK-GPUCommon-tests itkGPUImageFilterTest
        ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
=======
itk_module_test()

set(ITK-GPUCommon-tests
 itkGPUImageTest.cxx
 #itkGPUImageFilterTest.cxx
 #itkGPUImageFilterTestTemp.cxx
 itkGPUMeanImageFilterTest.cxx
 itkGPUBinaryThresholdImageFilterTest.cxx
 itkGPUGradientAnisotropicDiffusionImageFilterTest.cxx
 itkGPUDemonsRegistrationFilterTest.cxx
 )

include_directories(${OPENCL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/../)

CreateTestDriver(ITK-GPUCommon "${ITK-GPUCommon-Test_LIBRARIES}" "${ITK-GPUCommon-tests}" "${itkGPUHDR}")

add_test(NAME itkGPUImageFilterTest
        COMMAND ITK-GPUCommonTestDriver itkGPUImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
>>>>>>> b5d4d3e... WIP: Enable GPUCommon module, fix CMake bugs.
        ${TEMP}/gpuImageFilterTest.png)

#add_test(NAME itkGPUImageTest COMMAND ITK-GPUCommonTestDriver itkGPUImageTest)

#temporary test
#add_test(NAME itkGPUImageFilterTestTemp
#        COMMAND ITK-GPUCommonTestDriver itkGPUImageFilterTestTemp
#        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
#        ${TEMP}/gpuImageFilterTest.png)

add_test(NAME itkGPUMeanImageFilterTest
        COMMAND ITK-GPUCommonTestDriver itkGPUMeanImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${TEMP}/gpuMeanImageFilterTest.png)

add_test(NAME itkGPUBinaryThresholdImageFilterTest
        COMMAND ITK-GPUCommonTestDriver itkGPUBinaryThresholdImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${TEMP}/gpuBinaryThresholdImageFilterTest.png)

add_test(NAME itkGPUGradientAnisotropicDiffusionImageFilterTest
        COMMAND ITK-GPUCommonTestDriver itkGPUGradientAnisotropicDiffusionImageFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${TEMP}/gpuGradientAnisotropicDiffusionImageFilterTest.png)

add_test(NAME itkGPUDemonsRegistrationFilterTest
        COMMAND ITK-GPUCommonTestDriver itkGPUDemonsRegistrationFilterTest
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySlice.png
        ${TEMP}/gpuImageFilterTest.png
        ${TEMP}/output)


ENDIF(ITK_USE_GPU)
