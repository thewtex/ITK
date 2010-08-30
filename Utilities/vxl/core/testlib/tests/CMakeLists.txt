# This is core/testlib/tests/CMakeLists.txt

add_executable( testlib_test_all
   test_driver.cxx

   test_assert.cxx
   test_macros.cxx
   test_args.cxx
   test_root_dir.cxx
)
target_link_libraries( testlib_test_all itktestlib )

add_executable( testlib_test_link
   test_link.cxx
)
target_link_libraries( testlib_test_link itktestlib )

add_test( testlib_assert     ${EXECUTABLE_OUTPUT_PATH}/testlib_test_all test_assert )
add_test( testlib_macros     ${EXECUTABLE_OUTPUT_PATH}/testlib_test_all test_macros )
add_test( testlib_args       ${EXECUTABLE_OUTPUT_PATH}/testlib_test_all test_args one two )
add_test( testlib_root_dir   ${EXECUTABLE_OUTPUT_PATH}/testlib_test_all test_root_dir )
add_test( testlib_test_link  ${EXECUTABLE_OUTPUT_PATH}/testlib_test_link )
add_test( testlib_all        ${EXECUTABLE_OUTPUT_PATH}/testlib_test_all all one two )

add_executable( testlib_test_include test_include.cxx )
target_link_libraries( testlib_test_include itktestlib )
