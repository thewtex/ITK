set(ITKIOGDCM_SRC
itkGDCMImageIO.cxx
itkGDCMImageIOFactory.cxx
itkGDCMSeriesFileNames.cxx
)

add_library(ITKIOGDCM ${ITKIOGDCM_SRC})
target_link_libraries(ITKIOGDCM  ${ITK-GDCM_LIBRARIES} ${ITKIOBase_LIBRARIES} ${ITKImageIntensity_LIBRARIES})
itk_module_target(ITKIOGDCM)
