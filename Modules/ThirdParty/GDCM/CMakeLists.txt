project( ITK-GDCM )
set( ITK-GDCM_THIRD_PARTY 1 )

if( WIN32 )
  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 75 )
    message(
      FATAL_ERROR
      "Source tree directory of GDCM thirdparty module is too deep (${n} > 75)."
      "Please remove GDCM from the list of module to be built,"
      "or use a shorter directory for the ITK source tree."
      )
  endif()
endif()

if(ITK_USE_SYSTEM_GDCM)
  set(ITK-GDCM_SYSTEM_INCLUDE_DIRS
    ${GDCM_INCLUDE_DIRS}
    )
  set(ITK-GDCM_INCLUDE_DIRS ${GDCM_INCLUDE_DIRS})
  set(ITK-GDCM_LIBRARIES ITK-GDCM)
else()
  set(ITK-GDCM_INCLUDE_DIRS
    ${ITK-GDCM_BINARY_DIR}
    ${ITK-GDCM_BINARY_DIR}/src/gdcm/Source/Common
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/MediaStorageAndFileFormat
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/DataDictionary
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/Common
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/InformationObjectDefinition
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/MessageExchangeDefinition
    ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Source/DataStructureAndEncodingDefinition
    )
  include(CheckIncludeFile)
  check_include_file("stdint.h"       GDCM_HAVE_STDINT_H)
  if(NOT GDCM_HAVE_STDINT_H)
    # for MSVC only gdcm has a stdint.h that works for that
    # compiler found in Utilities/C99
    if(MSVC)
      list(APPEND ITK-GDCM_INCLUDE_DIRS
        ${ITK-GDCM_SOURCE_DIR}/src/gdcm/Utilities/C99)
    endif()
  endif()
  set(ITK-GDCM_LIBRARIES  gdcmDICT gdcmMSFF )
endif()

itk_module_impl()
