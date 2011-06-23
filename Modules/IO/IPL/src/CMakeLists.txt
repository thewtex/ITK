set(ITK_IO_IPL_SRC
itkIPLCommonImageIO.cxx
itkIPLFileNameList.cxx
)

add_library(ITK_IO_IPL ${ITK_IO_IPL_SRC})
target_link_libraries(ITK_IO_IPL  ${ITK_IO_Base_LIBRARIES})
itk_module_target(ITK_IO_IPL)
