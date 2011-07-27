# - Check the set of common warning flags supported by C and C++ compilers
# check_compiler_warning_flags(<c_flags_var> <cxx_flags_var>)
#  <c_flags_var> - variable to store valid C warning flags
#  <cxx_flags_var> - variable to store valid CXX warning flags
# This internally calls the check_c_compiler_flag and check_cxx_compiler_flag macros.


# To create a portable build system, it is best to not
# test for platforms, but to test for features.
#
# Instead of testing "if Windows then do this", test for
# "if the -Wno-invalid-offsetof flag works then use it".
#
# Typical use of this module is:
#
#  include(CheckCompilerWarningFlags)
#  check_compiler_warning_flags(C_WARNING_FLAGS CXX_WARNING_FLAGS)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARNING_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNING_FLAGS}")


include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)


function(check_c_compiler_warning_flags c_flag_var)
  set(local_c_flags "")
  set(flag_list "${ARGN}")
  foreach(flag IN LISTS flag_list)
    check_c_compiler_flag(${flag} C_HAS_WARNING${flag})
    if(${C_HAS_WARNING${flag}})
      set(local_c_flags "${local_c_flags} ${flag}")
    endif()
  endforeach()
  set(${c_flag_var} "${local_c_flags}" PARENT_SCOPE)
endfunction()


function(check_cxx_compiler_warning_flags cxx_flag_var)
  set(local_cxx_flags "")
  set(flag_list "${ARGN}")
  foreach(flag IN LISTS flag_list)
    check_cxx_compiler_flag(${flag} CXX_HAS_WARNING${flag})
    if(${CXX_HAS_WARNING${flag}})
      set(local_cxx_flags "${local_cxx_flags} ${flag}")
    endif()
  endforeach()
  set(${cxx_flag_var} "${local_cxx_flags}" PARENT_SCOPE)
endfunction()


function(check_compiler_warning_flags c_warning_flags_var cxx_warning_flags_var)
  set(${c_warning_flags_var} "" PARENT_SCOPE)
  set(${cxx_warning_flags_var} "" PARENT_SCOPE)

  # Check this list on C compiler only
  set(c_flags
    -Wno-uninitialized
    -Wno-unused-parameter
  )

  # Check this list on both C and C++ compilers
  set(c_and_cxx_flags
    -Wall
    -Wcast-align
    -Wdisabled-optimization
    -Wextra
    -Wformat=2
    -Winvalid-pch
    -Wlong-long
    -Wno-format-nonliteral
    -Wpointer-arith
    -Wshadow
    -Wunused
    -Wwrite-strings
  )

  # Check this list on C++ compiler only
  set(cxx_flags
    -Wno-deprecated
    -Wno-invalid-offsetof
    -Woverloaded-virtual
    -Wstrict-null-sentinel
  )

  check_c_compiler_warning_flags(CMAKE_C_WARNING_FLAGS ${c_flags} ${c_and_cxx_flags})
  check_cxx_compiler_warning_flags(CMAKE_CXX_WARNING_FLAGS ${c_and_cxx_flags} ${cxx_flags})

  set(${c_warning_flags_var} "${CMAKE_C_WARNING_FLAGS}" PARENT_SCOPE)
  set(${cxx_warning_flags_var} "${CMAKE_CXX_WARNING_FLAGS}" PARENT_SCOPE)
endfunction()
