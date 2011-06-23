project(ITK_GDCM)
set(ITK_GDCM_THIRD_PARTY 1)

if(ITK_USE_SYSTEM_GDCM)
  set(ITK_GDCM_SYSTEM_INCLUDE_DIRS
    ${GDCM_INCLUDE_DIRS}
    )
  set(ITK_GDCM_INCLUDE_DIRS ${GDCM_INCLUDE_DIRS})
  set(ITK_GDCM_LIBRARIES ITK_GDCM)
else()
  set(ITK_GDCM_INCLUDE_DIRS
    ${ITK_GDCM_BINARY_DIR}
    ${ITK_GDCM_BINARY_DIR}/src/gdcm/Source/Common
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/MediaStorageAndFileFormat
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/DataDictionary
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/Common
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/InformationObjectDefinition
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/MessageExchangeDefinition
    ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Source/DataStructureAndEncodingDefinition
    )
  include(CheckIncludeFile)
  check_include_file("stdint.h"       GDCM_HAVE_STDINT_H)
  if(NOT GDCM_HAVE_STDINT_H)
    # for MSVC only gdcm has a stdint.h that works for that
    # compiler found in Utilities/C99
    if(MSVC)
      list(APPEND ITK_GDCM_INCLUDE_DIRS
        ${ITK_GDCM_SOURCE_DIR}/src/gdcm/Utilities/C99)
    endif()
  endif()
  set(ITK_GDCM_LIBRARIES  gdcmDICT gdcmMSFF )
endif()

itk_module_impl()
