set(ITK_Blox_SRC
itkBloxItem.cxx
)

add_library(ITK_Blox ${ITK_Blox_SRC})
target_link_libraries(ITK_Blox  ${ITK_ImageIntensity_LIBRARIES} ${ITK_ImageGradient_LIBRARIES} ${ITK_Smoothing_LIBRARIES} ${ITK_Optimizers_LIBRARIES} ${ITK_IO_RAW_LIBRARIES})
itk_module_target(ITK_Blox)
