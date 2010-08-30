# TODO:
# SWIG is really a pain in the neck to use, a better alternative is Py++ which is using
# gccxml for the C++ parser and allow a full ANSI C++ support
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(
  "${GDCM_BINARY_DIR}/Source/Common"
  "${GDCM_SOURCE_DIR}/Source/Common"
  "${GDCM_SOURCE_DIR}/Source/DataStructureAndEncodingDefinition"
  "${GDCM_SOURCE_DIR}/Source/MediaStorageAndFileFormat"
  "${GDCM_SOURCE_DIR}/Source/DataDictionary"
)

find_package(PHP4 REQUIRED)

include_directories(
  ${PHP4_INCLUDE_PATH}
)

set_source_files_properties(gdcm.i PROPERTIES CPLUSPLUS ON)

# Some old swig 1.3 did not support this option:
#set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")

# swig expect a name like that: php_gdcm.so
SWIG_ADD_MODULE(php_gdcm php gdcm.i)
SWIG_LINK_LIBRARIES(php_gdcm gdcmMSFF)

#Module are always place in the library destination
#but for poot win32 user I decided to place them
# right next to the other dlls
if(NOT GDCM_INSTALL_NO_LIBRARIES)
  install(TARGETS ${SWIG_MODULE_gdcm_REAL_NAME}
    RUNTIME DESTINATION ${GDCM_INSTALL_BIN_DIR} COMPONENT Applications
    LIBRARY DESTINATION ${GDCM_INSTALL_LIB_DIR} COMPONENT Libraries
    ARCHIVE DESTINATION ${GDCM_INSTALL_LIB_DIR} COMPONENT Development
  ${CPACK_NAMELINK_TYPE}
  )
endif(NOT GDCM_INSTALL_NO_LIBRARIES)
