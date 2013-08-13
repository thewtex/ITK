option(ITK_USE_KWSTYLE "Enable the use of KWStyle for checking coding style." OFF)
mark_as_advanced(ITK_USE_KWSTYLE)
if(ITK_USE_KWSTYLE)

find_package(KWStyle 1.0.1
  QUIET MODULE
  REQUIRED
  )

option(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
mark_as_advanced(KWSTYLE_USE_VIM_FORMAT)
option(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." OFF)
mark_as_advanced(KWSTYLE_USE_MSVC_FORMAT)

if(${CMAKE_CXX_COMPILER} MATCHES "cl.exe$")
  set(KWSTYLE_USE_MSVC_FORMAT 1)
endif(${CMAKE_CXX_COMPILER} MATCHES "cl.exe$")

if(${CMAKE_C_COMPILER} MATCHES "g[cx][cx]$")
  set(KWSTYLE_USE_VIM_FORMAT 1)
endif(${CMAKE_C_COMPILER} MATCHES "g[cx][cx]$")

if(KWSTYLE_FOUND)
# Define and configure configuration files
set(KWSTYLE_CONFIGURATION_FILE
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITK.kws.xml)

set(KWSTYLE_ITK_FILES_LIST
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITKFiles.txt)

set(KWSTYLE_ITK_OVERWRITE_FILE
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITKOverwrite.txt )

configure_file(
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITK.kws.xml.in
  ${KWSTYLE_CONFIGURATION_FILE})

configure_file(
  ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITKFiles.txt.in
  ${KWSTYLE_ITK_FILES_LIST})


#  Define formatting for error messages
set(KWSTYLE_EDITOR_FORMAT "")

if(KWSTYLE_USE_VIM_FORMAT)
  set(KWSTYLE_EDITOR_FORMAT -vim)
endif(KWSTYLE_USE_VIM_FORMAT)

if(KWSTYLE_USE_MSVC_FORMAT)
  set(KWSTYLE_EDITOR_FORMAT -msvc)
endif(KWSTYLE_USE_MSVC_FORMAT)


# Add build target and CTest test
set(KWSTYLE_ARGUMENTS_CODE
  -xml ${KWSTYLE_CONFIGURATION_FILE} -v -D ${KWSTYLE_ITK_FILES_LIST}
  -o ${KWSTYLE_ITK_OVERWRITE_FILE} ${KWSTYLE_EDITOR_FORMAT}
  )

add_custom_target(StyleCheckCode
  COMMAND ${KWSTYLE_EXECUTABLE} ${KWSTYLE_ARGUMENTS_CODE}
  COMMENT "Coding Style Checker"
  )

if(BUILD_TESTING)
  set(itk-module KWStyle)
  itk_add_test(NAME KWStyleCodeTest
    COMMAND ${KWSTYLE_EXECUTABLE} ${KWSTYLE_ARGUMENTS_CODE}
    )
endif(BUILD_TESTING)

endif(KWSTYLE_FOUND)
endif(ITK_USE_KWSTYLE)
