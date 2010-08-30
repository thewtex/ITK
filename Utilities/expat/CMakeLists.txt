project(ITKEXPAT)
include_regular_expression("^(expat|xml|ascii|utf|name|iascii|latin|itk).*$")

set(expat_SRCS
  xmlparse.c
  xmltok.c
  xmlrole.c
)

include_directories(BEFORE ${ITKEXPAT_SOURCE_DIR})
include_directories(BEFORE ${ITKEXPAT_BINARY_DIR})

if(WIN32)
  if(NOT BUILD_SHARED_LIBS)
    set(ITK_EXPAT_STATIC 1)
  endif(NOT BUILD_SHARED_LIBS)
endif(WIN32)

configure_file(${ITKEXPAT_SOURCE_DIR}/expatConfig.h.in
               ${ITKEXPAT_BINARY_DIR}/expatConfig.h)
configure_file(${ITKEXPAT_SOURCE_DIR}/expatDllConfig.h.in
               ${ITKEXPAT_BINARY_DIR}/expatDllConfig.h)
configure_file(${ITKEXPAT_SOURCE_DIR}/.NoDartCoverage
               ${ITKEXPAT_BINARY_DIR}/.NoDartCoverage)

add_library(ITKEXPAT ${expat_SRCS})

# Apply user-defined properties to the library target.
if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(ITKEXPAT PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif(ITK_LIBRARY_PROPERTIES)

if(NOT ITK_INSTALL_NO_LIBRARIES)
  install(TARGETS ITKEXPAT
    EXPORT ITKTargets
    RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
endif(NOT ITK_INSTALL_NO_LIBRARIES)

if(ITK_USE_FLAT_DIRECTORY_INSTALL)
  set( ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_EXPAT ${ITK_INSTALL_INCLUDE_DIR_CM24})
else(ITK_USE_FLAT_DIRECTORY_INSTALL)
  set( ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_EXPAT ${ITK_INSTALL_INCLUDE_DIR_CM24}/Utilities/expat)
endif(ITK_USE_FLAT_DIRECTORY_INSTALL)


if(NOT ITK_INSTALL_NO_DEVELOPMENT)
  install(FILES
    ${ITKEXPAT_BINARY_DIR}/expatDllConfig.h
    ${ITKEXPAT_SOURCE_DIR}/expat.h
    DESTINATION ${ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_EXPAT}
    COMPONENT Development)
endif(NOT ITK_INSTALL_NO_DEVELOPMENT)
