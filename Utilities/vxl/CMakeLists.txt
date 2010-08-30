# Root vxl ported to ITK Utilities.
project(vxl)

include_regular_expression("^.*$")

if(NOT ITK_SOURCE_DIR)
  set(BUILD_TESTING 1)
  set(LIBRARY_OUTPUT_PATH ${vxl_BINARY_DIR}/bin)
  set(EXECUTABLE_OUTPUT_PATH ${vxl_BINARY_DIR}/bin)
endif(NOT ITK_SOURCE_DIR)

# CMake support directory.
include(${vxl_SOURCE_DIR}/config/cmake/config/vxl_utils.cmake)
set(VXL_CMAKE_DIR ${vxl_SOURCE_DIR}/config/cmake/Modules)
set(MODULE_PATH ${vxl_SOURCE_DIR}/config/cmake/Modules)

# Do platform-specific configuration.
subdirs(config/cmake/config)

# Build the core vxl + support libraries
subdirs(vcl v3p core)

# Standard include directories.
set(VXLCORE_INCLUDE_DIR ${vxl_BINARY_DIR}/core ${vxl_SOURCE_DIR}/core)
set(VXLCORE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/core)
set(VCL_INCLUDE_DIR ${vxl_BINARY_DIR}/vcl ${vxl_SOURCE_DIR}/vcl)
set(VCL_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/vcl)

include_directories(${VCL_INCLUDE_DIR} ${VXLCORE_INCLUDE_DIR})

set(VXL_INSTALL_ROOT ${ITK_INSTALL_INCLUDE_DIR}/Utilities/vxl)
set(VXL_INSTALL_LIB_DIR ${ITK_INSTALL_LIB_DIR})
set(VXL_INSTALL_BIN_DIR ${ITK_INSTALL_BIN_DIR})
set(VXL_INSTALL_NO_DEVELOPMENT ${ITK_INSTALL_NO_DEVELOPMENT})
set(VXL_INSTALL_NO_LIBRARIES ${ITK_INSTALL_NO_LIBRARIES})

string(REGEX REPLACE "^/" "" VXL_INSTALL_LIB_DIR_CM24 "${VXL_INSTALL_LIB_DIR}")
string(REGEX REPLACE "^/" "" VXL_INSTALL_BIN_DIR_CM24 "${VXL_INSTALL_BIN_DIR}")

if(WIN32)
  if(NOT UNIX)
    if(NOT BORLAND)
      if(NOT MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
        # Revert: /wd8004 /wd8057 produces warnings on .Net2003:
        #         invalid value '8004' for '/wd'; assuming '4999
        # you probably do not want to disable 4999, do you?
        # Which compiler were 8004 and 8057 referring to?
      endif(NOT MINGW)
    endif(NOT BORLAND)
  endif(NOT UNIX)
endif(WIN32)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif(CMAKE_COMPILER_IS_GNUCXX)

