cmake_policy(VERSION 2.8)

# This checks HeaderTest's in each module.  A HeaderTest can be found in the
# module 'test' directory in a file itk<module_name>HeaderTest.cxx.  This
# contains a null main(), but includes all the classes in the module.  The
# primary purpose of this test is to make sure there are not missing module
# dependencies.
#
# This script checks to make sure the HeaderTest's contain all of the module's
# headers.

# Use the cmake script independently (so it can be run in a unit test).
set( USAGE "usage: cmake -P ITKModuleHeaderTest.cmake /path/to/module" )

get_filename_component(_module_dir "${CMAKE_ARGV3}" ABSOLUTE)
if( "${CMAKE_ARGC}" LESS 4 OR NOT EXISTS "${_module_dir}")
  message( FATAL_ERROR ${USAGE} )
endif()

set( _include ${_module_dir}/include )
set( _test    ${_module_dir}/test )

# Check for the header test source code.
file( GLOB _header_test_src_list ${_test}/*HeaderTest*.cxx )
if( NOT _header_test_src_list )
  message( FATAL_ERROR "Module header test source code not found." )
endif()

# Check to make sure all the headers are in the header test.
file( GLOB _h_files RELATIVE   ${_include} ${_include}/*.h )
foreach( _header_file IN LISTS _h_files )

  set( _file_to_check ${_header_file} )
  # Use the .hxx if possible.
  get_filename_component( _header_filebase ${_header_file} NAME_WE )
  if( EXISTS ${_include}/${_header_filebase}.hxx )
    set( _file_to_check ${_header_filebase}.hxx )
  endif()

  set( _found_file FALSE )
  foreach( _header_test_src IN LISTS _header_test_src_list )
    file( STRINGS ${_header_test_src} _what_was_found REGEX ${_file_to_check} )
    if( _what_was_found )
      set( _found_file TRUE )
    endif()
  endforeach()

  if( NOT ${_found_file} )
    message( FATAL_ERROR "Did not find ${_file_to_check} in the HeaderTest for
    ${_module_dir}" )
  endif()
endforeach()
