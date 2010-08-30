# ./vxl/testlib/CMakeLists.txt

set(TESTLIB_VCL_WHERE_ROOT_DIR_H ${vxl_BINARY_DIR}/vcl/vcl_where_root_dir.h)

if(TESTLIB_VCL_WHERE_ROOT_DIR_H)
  add_definitions(-DVCL_WHERE_ROOT_DIR_H_EXISTS)
endif(TESTLIB_VCL_WHERE_ROOT_DIR_H)

set( testlib_sources

  # Useful utilities for the tests
  testlib_test.h                testlib_test.cxx

  # For use by the driver program
  testlib_register.h

  # The main function of the driver executable
  testlib_main.cxx

  # Used to locate test files in source tree
  testlib_root_dir.h            testlib_root_dir.cxx
)

add_library( itktestlib ${testlib_sources} )
target_link_libraries( itktestlib itkvcl )

if( BUILD_TESTING )
  subdirs( tests )
endif( BUILD_TESTING )
