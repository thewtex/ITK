<<<<<<< HEAD
create_test_sourcelist(Tests ITKGPUCommon-tests.cxx
itkGPUImageTest.cxx
itkGPUImageFilterTest.cxx
)

add_executable(ITKGPUCommon-tests  ${Tests} )
itk_module_target_label(ITKGPUCommon-tests)
target_link_libraries(ITKGPUCommon-tests  ${ITKGPUCommon_LIBRARIES} )

itk_add_test(NAME itkGPUImageFilterTest
        COMMAND ITKGPUCommon-tests itkGPUImageFilterTest
        ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
=======
IF (ITK_USE_GPU)

# With MS compilers on Win64, we need the /bigobj switch, else generated
# code results in objects with number of sections exceeding object file
# format.
# see http://msdn.microsoft.com/en-us/library/ms173499.aspx
if(CMAKE_CL_64 OR CMAKE_COMPILER_2005)
add_definitions(/bigobj)
endif()

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
>>>>>>> ENH: GPU Alpha
        ${TEMP}/gpuImageFilterTest.png)

#add_test(NAME itkGPUImageTest COMMAND ITK-GPUCommonTestDriver itkGPUImageTest)
#
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
