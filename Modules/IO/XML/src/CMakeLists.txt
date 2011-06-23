set(ITK_IO_XML_SRC
itkXMLFile.cxx
)

add_library(ITK_IO_XML ${ITK_IO_XML_SRC})
target_link_libraries(ITK_IO_XML  ${ITK_Expat_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_XML)
