#
# - Test CXX compiler for a flag
# Check if the CXX compiler accepts a flag
#
# ITK_CHECK_CXX_ACCEPTS_FLAGS(FLAGS VAR)
# - macro which checks if the code compiles with the given flags
#  FLAGS - cxx flags to try
#  VAR   - variable to store whether compiler accepts the FLAGS (TRUE or FALSE)
#


## Modified from CheckCXXCompilerFlag.cmake
include(CheckCXXSourceCompiles)
include(CheckCSourceCompiles)

## A thin wrapper that add a few extra FAIL_REGEX expressions
macro (ITK_CHECK_CXX_COMPILER_FLAG _FLAG _RESULT)
   set(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
   set(CMAKE_REQUIRED_DEFINITIONS "${_FLAG}")
   CHECK_CXX_SOURCE_COMPILES("int main() { return 0;}" ${_RESULT}
     # Some compilers do not fail with a bad flag
         FAIL_REGEX "unrecognized .*option"                     # GNU
         FAIL_REGEX "ignoring unknown option"                   # MSVC
         FAIL_REGEX "warning D9002"                             # MSVC, any lang
         FAIL_REGEX "warning:.*is a C++0x extension"            # clang
         FAIL_REGEX "[Uu]nknown option"                         # HP
         FAIL_REGEX "[Ww]arning: [Oo]ption"                     # SunPro
         FAIL_REGEX "command option .* is not recognized"       # XL
         FAIL_REGEX "Incorrect command line option"             # Any
     )
   set (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
endmacro (ITK_CHECK_CXX_COMPILER_FLAG)

## A thin wrapper that add a few extra FAIL_REGEX expressions
macro (ITK_CHECK_C_COMPILER_FLAG _FLAG _RESULT)
   set(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
   set(CMAKE_REQUIRED_DEFINITIONS "${_FLAG}")
   CHECK_C_SOURCE_COMPILES("int main() { return 0;}" ${_RESULT}
     # Some compilers do not fail with a bad flag
         FAIL_REGEX "unrecognized .*option"                     # GNU
         FAIL_REGEX "ignoring unknown option"                   # MSVC
         FAIL_REGEX "warning D9002"                             # MSVC, any lang
         FAIL_REGEX "warning:.*is a C++0x extension"            # clang
         FAIL_REGEX "[Uu]nknown option"                         # HP
         FAIL_REGEX "[Ww]arning: [Oo]ption"                     # SunPro
         FAIL_REGEX "command option .* is not recognized"       # XL
         FAIL_REGEX "Incorrect command line option"             # Any
     )
   set (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
endmacro (ITK_CHECK_C_COMPILER_FLAG)
