if(BUILD_TESTING)
  # test
  add_executable( netlib_lbfgs_example lbfgs-example.c )
  add_test( netlib_test_lbfgs ${EXECUTABLE_OUTPUT_PATH}/netlib_lbfgs_example )
  target_link_libraries(netlib_lbfgs_example itkv3p_netlib)
  # test
  add_executable( netlib_lbfgsb_example lbfgsb-example1.c )
  add_test( netlib_test_lbfgsb ${EXECUTABLE_OUTPUT_PATH}/netlib_lbfgsb_example )
  target_link_libraries(netlib_lbfgsb_example itkv3p_netlib)
  # test
  # FIXME   add_executable( netlib_lsqr_test lsqr-test.c )
  # FIXME   add_test( netlib_test_lsqr ${EXECUTABLE_OUTPUT_PATH}/netlib_lsqr_test )
  # FIXME   target_link_libraries(netlib_lsqr_test itkv3p_netlib)
  # test
  add_executable( netlib_slamch_test slamch-test.c )
  add_test( netlib_test_slamch ${EXECUTABLE_OUTPUT_PATH}/netlib_slamch_test )
  target_link_libraries(netlib_slamch_test itkv3p_netlib)
  # test
  add_executable( netlib_integral_test integral-test.c )
  add_test( netlib_test_integral ${EXECUTABLE_OUTPUT_PATH}/netlib_integral_test )
  target_link_libraries(netlib_integral_test itkv3p_netlib)
endif(BUILD_TESTING)
