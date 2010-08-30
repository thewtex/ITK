subdirs( Numerics Common BasicFilters Algorithms IO SpatialObject )

if(ITK_USE_PATENTED)
  subdirs(Patented)
endif(ITK_USE_PATENTED)

if(ITK_USE_REVIEW)
  subdirs(Review)
endif(ITK_USE_REVIEW)

if(ITK_BINARY_DIR)
  set(ITK_TEST_OUTPUT_DIR "${ITK_BINARY_DIR}/Testing/Temporary")
  if(UNIX)
    if("x${CMAKE_INSTALL_PREFIX}" STREQUAL "^x${ITK_BINARY_DIR}/InstallTest$")
      configure_file(${ITKTesting_SOURCE_DIR}/Code/install_test.sh.in
                     ${ITKTesting_BINARY_DIR}/Code/install_test.sh @ONLY
                     IMMEDIATE)
      add_test(Install /bin/sh ${ITKTesting_BINARY_DIR}/Code/install_test.sh)
    endif("x${CMAKE_INSTALL_PREFIX}" STREQUAL "^x${ITK_BINARY_DIR}/InstallTest$")
  endif(UNIX)
else(ITK_BINARY_DIR)
  set(ITK_TEST_OUTPUT_DIR "${ITKTesting_BINARY_DIR}/Testing/Temporary")
endif(ITK_BINARY_DIR)
