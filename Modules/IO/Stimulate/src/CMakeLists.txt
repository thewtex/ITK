set(ITK_IO_Stimulate_SRC
itkStimulateImageIO.cxx
itkStimulateImageIOFactory.cxx
)

add_library(ITK_IO_Stimulate ${ITK_IO_Stimulate_SRC})
target_link_libraries(ITK_IO_Stimulate  ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_Stimulate)
