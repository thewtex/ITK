# This is core/vnl/algo/tests/CMakeLists.txt

include( ${MODULE_PATH}/FindNetlib.cmake )

# message( NETLIB_FOUND " is " ${NETLIB_FOUND} )
if(NETLIB_FOUND)

  include_directories( ${NETLIB_INCLUDE_DIR} )

  add_executable( vnl_algo_test_all
    # Driver source and utilities
    test_driver.cxx
    test_util.cxx       test_util.h

    # The tests
    test_algo.cxx
    test_amoeba.cxx
    test_cholesky.cxx
    test_complex_eigensystem.cxx
    #test_convolve.cxx # Removing for ITK: needs vul
    test_cpoly_roots.cxx
    test_determinant.cxx
    test_fft.cxx
    test_fft1d.cxx
    test_fft2d.cxx
    test_functions.cxx
    test_generalized_eigensystem.cxx
    test_ldl_cholesky.cxx
    test_levenberg_marquardt.cxx
    test_matrix_update.cxx
    test_minimizers.cxx
    test_powell.cxx
    test_qr.cxx
    test_qsvd.cxx
    test_rank.cxx
    test_real_eigensystem.cxx
    test_rnpoly_roots.cxx
    test_sparse_matrix.cxx
    test_svd.cxx
    #test_symmetric_eigensystem.cxx # Removing for ITK: needs vul
    test_integral.cxx
    test_solve_qp.cxx
    test_sparse_lu.cxx
    test_bracket_minimum.cxx
    test_brent_minimizer.cxx
  )


  target_link_libraries( vnl_algo_test_all itkvnl_algo itktestlib ${CMAKE_THREAD_LIBS} )


  add_test( vnl_test_algo                     ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_algo                    )
  add_test( vnl_test_amoeba                   ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_amoeba                  )
  add_test( vnl_test_cholesky                 ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_cholesky                )
  add_test( vnl_test_complex_eigensystem      ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_complex_eigensystem     )
  #add_test( vnl_test_convolve                 ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_convolve                )
  add_test( vnl_test_cpoly_roots              ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_cpoly_roots             )
  add_test( vnl_test_determinant              ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_determinant             )
  add_test( vnl_test_fft                      ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_fft                     )
  add_test( vnl_test_fft1d                    ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_fft1d                   )
  add_test( vnl_test_fft2d                    ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_fft2d                   )
  add_test( vnl_test_functions                ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_functions               )
  add_test( vnl_test_generalized_eigensystem  ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_generalized_eigensystem )
  add_test( vnl_test_ldl_cholesky             ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_ldl_cholesky                )
  add_test( vnl_test_levenberg_marquardt      ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_levenberg_marquardt     )
  add_test( vnl_test_matrix_update            ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_matrix_update              )
  add_test( vnl_test_minimizers               ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_minimizers              )
  add_test( vnl_test_powell                   ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_powell                  )
  add_test( vnl_test_qr                       ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_qr                      )
  add_test( vnl_test_qsvd                     ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_qsvd                    )
  add_test( vnl_test_rank                     ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_rank                    )
  add_test( vnl_test_real_eigensystem         ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_real_eigensystem        )
  add_test( vnl_test_rnpoly_roots             ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_rnpoly_roots            )
  add_test( vnl_test_integral                 ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_integral                )
  add_test( vnl_test_solve_qp                 ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_solve_qp                )
  add_test( vnl_test_sparse_lu                ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_sparse_lu               )
  add_test( vnl_test_bracket_minimum          ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_bracket_minimum         )
  add_test( vnl_test_brent_minimizer          ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_brent_minimizer         )

  if( SITE MATCHES "isbe.man.ac.uk" )
    # For some reason my box (linux-gcc-3.2) has a problem with the optimisation
    # settings for dnlaso.c in netlib. It cannot be fixed until > CMAKE 1.8.3
  else( SITE MATCHES "isbe.man.ac.uk" )
    add_test( vnl_test_sparse_matrix            ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_sparse_matrix           )
  endif( SITE MATCHES "isbe.man.ac.uk" )
  add_test( vnl_test_svd                      ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_svd                     )
  #add_test( vnl_test_symmetric_eigensystem    ${EXECUTABLE_OUTPUT_PATH}/vnl_algo_test_all test_symmetric_eigensystem   )
endif(NETLIB_FOUND)

# GCC 2.95 has problems when compiling test_algo.cxx with "-O2" flag.
# The solution is to turn off optimization for GCC < 3.0
if(CMAKE_COMPILER_IS_GNUCXX)
  if( VNL_COMPILER_IS_GNUCXX_2XX MATCHES "VNL_COMPILER_IS_GNUCXX_2XX")
    exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
    if(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
      set( VNL_COMPILER_IS_GNUCXX_2XX 1 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    else(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
      set( VNL_COMPILER_IS_GNUCXX_2XX 0 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    endif(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
  endif( VNL_COMPILER_IS_GNUCXX_2XX MATCHES "VNL_COMPILER_IS_GNUCXX_2XX")

  if(VNL_COMPILER_IS_GNUCXX_2XX)
    # We only need to remove "-O2" flag from test_algo.cxx.
    # But it is much easier to do it for all
    foreach(var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG
                CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
      string(REGEX REPLACE "-O2" "" "${var}" "${${var}}")
    endforeach(var)
  endif(VNL_COMPILER_IS_GNUCXX_2XX)
endif( CMAKE_COMPILER_IS_GNUCXX )

add_executable( vnl_algo_test_include test_include.cxx )
target_link_libraries( vnl_algo_test_include itkvnl_algo )
