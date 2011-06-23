set(ITK_IO_GE_SRC
itkGE5ImageIOFactory.cxx
itkGEAdwImageIO.cxx
itkGE4ImageIO.cxx
itkGEAdwImageIOFactory.cxx
itkGE5ImageIO.cxx
itkGE4ImageIOFactory.cxx
)

add_library(ITK_IO_GE ${ITK_IO_GE_SRC})
target_link_libraries(ITK_IO_GE  ${ITK_IO_IPL_LIBRARIES})
itk_module_target(ITK_IO_GE)
