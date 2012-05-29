
set(ITKIODCMTK_SRC
  itkDCMTKImageIO.cxx
  itkDCMTKImageIOFactory.cxx
)

add_library(ITKIODCMTK ${ITKIODCMTK_SRC})
target_link_libraries(ITKIODCMTK ${DCMTK_LIBRARIES}
  ${ITKIOImageBase_LIBRARIES} ${ITKImageIntensity_LIBRARIES})
itk_module_target(ITKIODCMTK)
