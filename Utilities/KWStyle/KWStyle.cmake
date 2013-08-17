option(ITK_USE_KWSTYLE
  "Enable the use of KWStyle for checking coding style."
  OFF
  )
mark_as_advanced(ITK_USE_KWSTYLE)

if(ITK_USE_KWSTYLE)
  find_package(KWStyle 1.0.1
    QUIET MODULE
    REQUIRED
    )

  if(KWSTYLE_FOUND)
    # Define and configure configuration files
    set(KWSTYLE_CONFIGURATION_FILE
      ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITK.kws.xml
      )
    configure_file(
      ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITK.kws.xml.in
      ${KWSTYLE_CONFIGURATION_FILE}
      )
    set(KWSTYLE_ITK_FILES_LIST
      ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITKFiles.txt
      )
    configure_file(
      ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITKFiles.txt.in
      ${KWSTYLE_ITK_FILES_LIST}
      )
    set(KWSTYLE_ITK_OVERWRITE_FILE
      ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/ITKOverwrite.txt
      )

    # Define formatting for error messages
    option(KWSTYLE_USE_MSVC_FORMAT
      "Set KWStyle to generate errors with a Visual Studio-compatible format."
      ${MSVC_IDE} # default to ON only with a Visual Studio IDE
      )
    mark_as_advanced(KWSTYLE_USE_MSVC_FORMAT)
    option(KWSTYLE_USE_VIM_FORMAT
      "Set KWStyle to generate errors with a VIM-compatible format."
      OFF # VIM-style output may not always be desirable on Unix or with GCC,
          # and we can't easily determine if a build is being done though VIM,
          # so default to off
      )
    mark_as_advanced(KWSTYLE_USE_VIM_FORMAT)
    if(KWSTYLE_USE_MSVC_FORMAT AND KWSTYLE_USE_VIM_FORMAT)
      message(FATAL_ERROR
        "Options KWSTYLE_USE_MSVC_FORMAT and KWSTYLE_USE_VIM_FORMAT cannot both be set to TRUE."
        )
    elseif(KWSTYLE_USE_MSVC_FORMAT)
      set(KWSTYLE_EDITOR_FORMAT -msvc)
    elseif(KWSTYLE_USE_VIM_FORMAT)
      set(KWSTYLE_EDITOR_FORMAT -vim)
    else()
      set(KWSTYLE_EDITOR_FORMAT "")
    endif()

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
