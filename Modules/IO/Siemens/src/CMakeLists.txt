set(ITK_IO_Siemens_SRC
itkSiemensVisionImageIOFactory.cxx
itkSiemensVisionImageIO.cxx
)

add_library(ITK_IO_Siemens ${ITK_IO_Siemens_SRC})
target_link_libraries(ITK_IO_Siemens  ${ITK_IO_IPL_LIBRARIES} ${ITK_IO_GE_LIBRARIES})
itk_module_target(ITK_IO_Siemens)
