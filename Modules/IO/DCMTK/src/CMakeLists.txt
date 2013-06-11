set(ITKIODCMTK_SRC
  itkDCMTKImageIO.cxx
  itkDCMTKFileReader.cxx
  itkDCMTKImageIOFactory.cxx
  itkDCMTKSeriesFileNames.cxx
)

add_library(ITKIODCMTK ${ITK_LIBRARY_BUILD_TYPE} ${ITKIODCMTK_SRC})

target_link_libraries(ITKIODCMTK ${ITKDCMTK_LIBRARIES}
  ${ITKIOImageBase_LIBRARIES} ${ITKImageIntensity_LIBRARIES})

itk_module_target(ITKIODCMTK)
