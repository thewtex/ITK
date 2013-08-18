# - Finds the KWStyle utility
# This module looks for KWStyle. If KWStyle is found,
# the following variables are defined:
#  KWSTYLE_FOUND - Set if KWStyle is found
#  KWSTYLE_EXECUTABLE - Path to the KWStyle executable
#  KWSTYLE_VERSION_STRING - A human-readable string containing the version of KWStyle

#=============================================================================
# Copyright 2008-2013 Kitware, Inc.
# Copyright 2013 Brian Helba
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

find_program(
  KWSTYLE_EXECUTABLE
  NAMES KWStyle
  DOC "Path to the KWStyle executable"
  )

if(KWSTYLE_EXECUTABLE)
  execute_process(
    COMMAND ${KWSTYLE_EXECUTABLE} -version
    OUTPUT_VARIABLE KWSTYLE_VERSION_STRING
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  string(REPLACE
    "Version: "
    ""
    KWSTYLE_VERSION_STRING
    ${KWSTYLE_VERSION_STRING}
    )
endif(KWSTYLE_EXECUTABLE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  KWStyle
  REQUIRED_VARS KWSTYLE_EXECUTABLE
  VERSION_VAR KWSTYLE_VERSION_STRING
  )
