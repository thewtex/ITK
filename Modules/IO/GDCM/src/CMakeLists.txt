set(ITK_IO_GDCM_SRC
itkGDCMImageIO.cxx
itkGDCMImageIOFactory.cxx
itkGDCMSeriesFileNames.cxx
)

add_library(ITK_IO_GDCM ${ITK_IO_GDCM_SRC})
target_link_libraries(ITK_IO_GDCM  ${ITK_GDCM_LIBRARIES} ${ITK_IO_Base_LIBRARIES} ${ITK_ImageIntensity_LIBRARIES})
itk_module_target(ITK_IO_GDCM)
