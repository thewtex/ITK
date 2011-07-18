
configure_file(itkBioFormatsImageIO.cxx.in ${CMAKE_CURRENT_BINARY_DIR}/itkBioFormatsImageIO.cxx @ONLY)

set(ITKIOBioFormats_SRC
itkBioFormatsImageIOFactory.cxx
${CMAKE_CURRENT_BINARY_DIR}/itkBioFormatsImageIO.cxx
)

add_library(ITKIOBioFormats ${ITKIOBioFormats_SRC})
target_link_libraries(ITKIOBioFormats  ${ITKIOBase_LIBRARIES})
itk_module_target(ITKIOBioFormats)
