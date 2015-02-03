project(ITKOpenJPEG)
set(ITKOpenJPEG_THIRD_PARTY 1)

set(ITKOpenJPEG_INCLUDE_DIRS
  ${ITKOpenJPEG_BINARY_DIR}/src/openjpeg/src/lib/openjp2
  ${ITKOpenJPEG_SOURCE_DIR}/src/openjpeg/src/lib/openjp2
  )
set(ITKOpenJPEG_LIBRARIES itkopenjp2)

itk_module_impl()

install(FILES ${ITKOpenJPEG_SOURCE_DIR}/src/itk_openjpeg.h
  DESTINATION ${ITKOpenJPEG_INSTALL_INCLUDE_DIR}
  COMPONENT Development
  )
