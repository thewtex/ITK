# This is core/vnl/tests/CMakeLists.txt

add_executable( vnl_test_all
  # Driver source and utilities
  test_driver.cxx

  # The tests
  test_bignum.cxx
  test_complex.cxx
  test_inverse.cxx
  test_diag_matrix.cxx
  test_file_matrix.cxx
  test_finite.cxx
  test_math.cxx
  #test_matlab.cxx # Removing for ITK: needs vul and vpl
  test_matrix.cxx
  test_matrix_exp.cxx
  test_matrix_fixed.cxx
  test_vector_fixed_ref.cxx
  test_matrix_fixed_ref.cxx
  test_numeric_traits.cxx
  test_quaternion.cxx
  test_rational.cxx
  test_real_polynomial.cxx
  test_real_npolynomial.cxx
  test_resize.cxx
  test_rotation_matrix.cxx
  test_sample.cxx
  test_sym_matrix.cxx
  test_transpose.cxx
  test_fastops.cxx
  test_vector.cxx
  test_gamma.cxx
  test_random.cxx
  test_arithmetic.cxx  test_arithmetic_body.h
  test_hungarian_algorithm.cxx
  test_integrant.cxx
  test_bessel.cxx
)

if(CMAKE_COMPILER_IS_GNUCXX)
 set_source_files_properties(test_rational.cxx PROPERTIES COMPILE_FLAGS -O1)
 set_source_files_properties(test_bignum.cxx PROPERTIES COMPILE_FLAGS -O3)
 set_source_files_properties(test_finite.cxx PROPERTIES COMPILE_FLAGS -O0)
endif(CMAKE_COMPILER_IS_GNUCXX)

target_link_libraries(vnl_test_all itkvnl itktestlib ${CMAKE_THREAD_LIBS})

add_executable(vnl_basic_operation_timings basic_operation_timings.cxx)
target_link_libraries(vnl_basic_operation_timings itkvnl)

add_test( vnl_test_bignum               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_bignum               )
add_test( vnl_test_complex              ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_complex              )
add_test( vnl_test_diag_matrix          ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_diag_matrix          )
add_test( vnl_test_file_matrix          ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_file_matrix          )
add_test( vnl_test_finite               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_finite               )
add_test( vnl_test_inverse              ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_inverse              )
add_test( vnl_test_math                 ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_math                 )
#add_test( vnl_test_matlab               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_matlab               )
add_test( vnl_test_matrix               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_matrix               )
add_test( vnl_test_matrix_exp           ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_matrix_exp           )
add_test( vnl_test_matrix_fixed         ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_matrix_fixed         )
add_test( vnl_test_vector_fixed_ref     ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_vector_fixed_ref     )
add_test( vnl_test_matrix_fixed_ref     ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_matrix_fixed_ref     )
add_test( vnl_test_numeric_traits       ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_numeric_traits       )
add_test( vnl_test_random               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_random               )
add_test( vnl_test_rational             ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_rational             )
add_test( vnl_test_real_polynomial      ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_real_polynomial      )
add_test( vnl_test_real_npolynomial     ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_real_npolynomial     )
add_test( vnl_test_resize               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_resize               )
add_test( vnl_test_rotation_matrix      ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_rotation_matrix      )
add_test( vnl_test_sample               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_sample               )
add_test( vnl_test_sym_matrix           ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_sym_matrix           )
add_test( vnl_test_transpose            ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_transpose            )
add_test( vnl_test_fastops              ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_fastops              )
add_test( vnl_test_vector               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_vector               )
add_test( vnl_test_gamma                ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_gamma                )
add_test( vnl_test_arithmetic           ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_arithmetic           )
add_test( vnl_test_hungarian_algorithm  ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_hungarian_algorithm  )
add_test( vnl_test_integrant            ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_integrant            )
add_test( vnl_test_bessel               ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_bessel               )
add_test( vnl_test_quaternion           ${EXECUTABLE_OUTPUT_PATH}/vnl_test_all test_quaternion           )

add_executable(vnl_test_include test_include.cxx)
target_link_libraries(vnl_test_include itkvnl_algo itkvnl)
