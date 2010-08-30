if(WIN32)
  if(NOT MINGW)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/AllowContinuous.bat.in
                 ${ITK_BINARY_DIR}/AllowContinuous.bat)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/PreventContinuous.bat.in
                 ${ITK_BINARY_DIR}/PreventContinuous.bat)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuous.bat.in
                 ${ITK_BINARY_DIR}/BuildContinuous.bat)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuousWithCTest.bat.in
                 ${ITK_BINARY_DIR}/BuildContinuousWithCTest.bat)
  else(NOT MINGW)
    configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuous.sh.in
                   ${ITK_BINARY_DIR}/BuildContinuous.sh
                   @ONLY IMMEDIATE)
    exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/BuildContinuous.sh\"
                       OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
    configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuousWithCTest.sh.in
                   ${ITK_BINARY_DIR}/BuildContinuousWithCTest.sh
                   @ONLY IMMEDIATE)
    exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/BuildContinuousWithCTest.sh\"
                       OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
    configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/AllowContinuous.sh.in
                   ${ITK_BINARY_DIR}/AllowContinuous.sh
                   @ONLY IMMEDIATE)
    exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/AllowContinuous.sh\"
                       OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
    configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/PreventContinuous.sh.in
                   ${ITK_BINARY_DIR}/PreventContinuous.sh
                   @ONLY IMMEDIATE)
    exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/PreventContinuous.sh\"
                       OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
    configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/cronentry.in
                   ${ITK_BINARY_DIR}/cronentry
                   @ONLY IMMEDIATE)
  endif(NOT MINGW)
endif(WIN32)
if(UNIX)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuous.sh.in
                 ${ITK_BINARY_DIR}/BuildContinuous.sh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/BuildContinuous.sh\"
                     OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/BuildContinuousWithCTest.sh.in
                 ${ITK_BINARY_DIR}/BuildContinuousWithCTest.sh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/BuildContinuousWithCTest.sh\"
                       OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/AllowContinuous.sh.in
                 ${ITK_BINARY_DIR}/AllowContinuous.sh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/AllowContinuous.sh\"
                     OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/PreventContinuous.sh.in
                 ${ITK_BINARY_DIR}/PreventContinuous.sh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/PreventContinuous.sh\"
                     OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/cronentry.in
                 ${ITK_BINARY_DIR}/cronentry
                 @ONLY IMMEDIATE)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/NewSince.csh.in
                 ${ITK_BINARY_DIR}/NewSince.csh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/NewSince.csh\"
                 OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
  configure_file(${ITK_SOURCE_DIR}/Utilities/Dart/ChangedSince.csh.in
                 ${ITK_BINARY_DIR}/ChangedSince.csh
                 @ONLY IMMEDIATE)
  exec_program(chmod ARGS 755 \"${ITK_BINARY_DIR}/ChangedSince.csh\"
                 OUTPUT_VARIABLE ITK_EXEC_PLEASE_BE_QUIET)
endif(UNIX)
