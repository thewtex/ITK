## BioFormats is available at
##
##       http://www.loci.wisc.edu/bio-formats/source-code
##
## This file defines the following symbols
##
##  BIOFORMATS_INCLUDE_DIR   - Directories to include BioFormats CPP headers
##  BIOFORMATS_LIBRARIES     - Files to link against to use BioFormats CPP bindings
##  BIOFORMATS_LIB_DIR       - Directory where the libraries are
##  BIOFORMATS_FOUND         - If false, don't try to use BioFormats
##

set(BIOFORMATS_INC_SEARCHPATH
  /sw/include
  /usr/include
  /usr/local/include
)

find_path(BIOFORMATS_INCLUDE_PATH bio-formats.h ${BIOFORMATS_INC_SEARCHPATH})

if(BIOFORMATS_INCLUDE_PATH)
  set(BIOFORMATS_INCLUDE ${BIOFORMATS_INCLUDE_PATH})
endif(BIOFORMATS_INCLUDE_PATH)

if(BIOFORMATS_INCLUDE)
  include_directories(
    ${BIOFORMATS_INCLUDE}
    ${BIOFORMATS_INCLUDE}/../proxies/include
    )
endif(BIOFORMATS_INCLUDE)

find_path(JACE_INCLUDE_PATH JNIHelper.h ${BIOFORMATS_INC_SEARCHPATH})

if(JACE_INCLUDE_PATH)
  set(JACE_INCLUDE ${JACE_INCLUDE_PATH})
else()
  message(FATAL_ERROR "JACE_INCLUDE_PATH is not set and it is required to use BioFormats")
endif()

if(JACE_INCLUDE)
  include_directories( ${JACE_INCLUDE} )
endif(JACE_INCLUDE)

get_filename_component(JACE_INSTALL_BASE_PATH ${JACE_INCLUDE_PATH} PATH)

find_package( JNI REQUIRED )
include_directories( ${JNI_INCLUDE_DIRS} )

get_filename_component(BIOFORMATS_INSTALL_BASE_PATH ${BIOFORMATS_INCLUDE_PATH} PATH)

set(BIOFORMATS_LIB_SEARCHPATH
  ${BIOFORMATS_INSTALL_BASE_PATH}/lib
  /usr/lib/
  /usr/local/lib/
)

mark_as_advanced(BIOFORMATS_CPP_LIB)
find_library(BIOFORMATS_CPP_LIB bfcpp ${BIOFORMATS_LIB_SEARCHPATH})

mark_as_advanced(BIOFORMATS_JACE_LIB)
find_library(BIOFORMATS_JACE_LIB jace ${BIOFORMATS_LIB_SEARCHPATH})

if(BIOFORMATS_CPP_LIB AND BIOFORMATS_JACE_LIB)
  set(BIOFORMATS_FOUND 1)
  get_filename_component(BIOFORMATS_LIB_DIR ${BIOFORMATS_CPP_LIB} PATH)
  mark_as_advanced(BIOFORMATS_LIBRARIES)
  set(BIOFORMATS_LIBRARIES  bfcpp jace)
  message("BIOFORMATS_LIB_DIR" ${BIOFORMATS_LIB_DIR} )
  link_directories( ${BIOFORMATS_LIB_DIR} )
endif()
