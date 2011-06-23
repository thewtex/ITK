create_test_sourcelist(Tests ITK_GPUCommon-tests.cxx
itkGPUImageTest.cxx
itkGPUImageFilterTest.cxx
)

add_executable(ITK_GPUCommon-tests  ${Tests} )
itk_module_target_label(ITK_GPUCommon-tests)
target_link_libraries(ITK_GPUCommon-tests  ${ITK_GPUCommon_LIBRARIES} )

itk_add_test(NAME itkGPUImageFilterTest
        COMMAND ITK_GPUCommon-tests itkGPUImageFilterTest
        ${ITK_EXAMPLE_DATA_ROOT}/BrainProtonDensitySlice.png
        ${TEMP}/gpuImageFilterTest.png)
