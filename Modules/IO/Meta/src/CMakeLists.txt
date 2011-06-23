set(ITK_IO_Meta_SRC
itkMetaArrayWriter.cxx
itkMetaImageIO.cxx
itkMetaArrayReader.cxx
itkMetaImageIOFactory.cxx
)

add_library(ITK_IO_Meta ${ITK_IO_Meta_SRC})
target_link_libraries(ITK_IO_Meta  ${ITK_MetaIO_LIBRARIES} ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_Meta)
