set(ITKIOCSV_SRC
itkCSVFileReaderBase.cxx
)

add_library(ITKIOCSV ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOCSV_SRC})
target_link_libraries(ITKIOCSV  ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOCSV)
