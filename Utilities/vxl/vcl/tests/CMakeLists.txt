# vcl/tests/CMakeLists.txt

# Avoid adding more test executables here, in order to keep the number
# of projects small. (This is a concern for IDEs like Visual Studio.)
# Try to follow one of the other tests, like test_algorithm.cxx,
# and add the test into test_driver.cxx.

add_executable( vcl_test_all
 # Driver
     test_driver.cxx
 # The actual tests
     test_algorithm.cxx
     test_cctype.cxx
     test_cmath.cxx
     test_compiler.cxx
     test_complex.cxx
     test_deque.cxx
     test_exception.cxx
     test_fstream.cxx
     test_iostream.cxx
     test_iterator.cxx
     test_list.cxx
     test_limits.cxx
     test_map.cxx
     #test_memory.cxx
     test_multimap.cxx
     test_new.cxx
     test_set.cxx
     test_stlfwd.cxx
     test_string.cxx
     test_sstream.cxx
     test_vector.cxx
     test_cstdio.cxx
     test_preprocessor.cxx
)
target_link_libraries( vcl_test_all itkvcl )

add_test( vcl_test_algorithm    ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_algorithm )
add_test( vcl_test_cctype       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_cctype )
add_test( vcl_test_cmath        ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_cmath )
add_test( vcl_test_compiler     ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_compiler )
add_test( vcl_test_complex      ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_complex )
add_test( vcl_test_deque        ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_deque )
add_test( vcl_test_exception    ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_exception )
add_test( vcl_test_fstream      ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_fstream )
add_test( vcl_test_iostream     ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_iostream )
add_test( vcl_test_iterator     ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_iterator )
add_test( vcl_test_list         ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_list )
add_test( vcl_test_limits       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_limits )
add_test( vcl_test_map          ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_map )
#add_test( vcl_test_memory       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_memory )
add_test( vcl_test_multimap     ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_multimap )
add_test( vcl_test_new          ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_new )
add_test( vcl_test_set          ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_set )
add_test( vcl_test_string       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_string )
add_test( vcl_test_sstream      ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_sstream )
add_test( vcl_test_vector       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_vector )
add_test( vcl_test_cstdio       ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_cstdio ${CMAKE_CURRENT_SOURCE_DIR}/test_cstdio.txt )
add_test( vcl_test_preprocessor ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_preprocessor )
# Only a compiler test:
#add_test( vcl_test_stlfwd      ${EXECUTABLE_OUTPUT_PATH}/vcl_test_all test_stlfwd )

add_executable( vcl_test_include test_include.cxx )
target_link_libraries( vcl_test_include itkvcl )
