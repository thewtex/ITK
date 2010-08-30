# v3p/netlib/linalg/CMakeLists.txt

project( lsqr )

# List sources for each library component.
set(V3P_NETLIB_lsqr_SOURCES
  lsqrBase.h
  lsqrBase.cxx
  )

# Create a library .
add_library(itkv3p_lsqr ${V3P_NETLIB_lsqr_SOURCES})
if(UNIX)
  target_link_libraries( itkv3p_lsqr m )
endif(UNIX)
if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(itkv3p_lsqr PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif(ITK_LIBRARY_PROPERTIES)

if(NOT VXL_INSTALL_NO_LIBRARIES)
  install(TARGETS itkv3p_lsqr
    RUNTIME DESTINATION ${VXL_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${VXL_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${VXL_INSTALL_LIB_DIR_CM24} COMPONENT Development)
endif(NOT VXL_INSTALL_NO_LIBRARIES)
if(NOT VXL_INSTALL_NO_DEVELOPMENT)
  INSTALL_NOBASE_HEADER_FILES(${VXL_INSTALL_ROOT}/v3p/netlib ${V3P_NETLIB_lsqr_SOURCES})
endif(NOT VXL_INSTALL_NO_DEVELOPMENT)
