set(ITK_Watersheds_SRC
itkWatershedMiniPipelineProgressCommand.cxx
itkOneWayEquivalencyTable.cxx
)

add_library(ITK_Watersheds ${ITK_Watersheds_SRC})
target_link_libraries(ITK_Watersheds  ${ITK_ImageIntensity_LIBRARIES} ${ITK_Thresholding_LIBRARIES} ${ITK_ImageGradient_LIBRARIES} ${ITK_Smoothing_LIBRARIES})
itk_module_target(ITK_Watersheds)
