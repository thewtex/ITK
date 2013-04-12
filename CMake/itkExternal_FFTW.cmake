#
# Encapsulates building FFTW as an External Project.
include(ITK_CheckCCompilerFlag)


set(msg "ATTENTION: You have enabled the use of fftw,")
set(msg "${msg} this library is distributed under a GPL license.")
set(msg "${msg} By enabling this option, the binary of the ITK libraries")
set(msg "${msg} that you are going to build will be covered by a GPL license,")
set(msg "${msg} and so it will be any executable that you link against these libraries.")
message("${msg}")

#--check_c_compiler_flag(-fopenmp C_HAS_fopenmp)
#--if(${C_HAS_fopenmp} AND FALSE)
#--    set(FFTW_THREADS_CONFIGURATION --enable-openmp)
#--    set(OPENMP_FLAG "-fopenmp")
#--  else()
    set(FFTW_THREADS_CONFIGURATION --enable-threads)
    set(OPENMP_FLAG "")
#--endif()


## Perhaps in the future a set of TryCompiles could be used here.
set(FFTW_OPTIMIZATION_CONFIGURATION "" CACHE INTERNAL "architecture flags: --enable-sse --enable-sse2 --enable-altivec --enable-mips-ps --enable-cell")
if(ITK_USE_SYSTEM_FFTW)
  find_package( FFTW )
  link_directories(${FFTW_LIBDIR})
else()

  if(WIN32 AND NOT MINGW)
    message("Can't build fftw as external project on Windows")
    message(ERROR "install fftw and use ITK_USE_SYSTEM_FFTW")
  else()
    #
    # fftw limitation -- can't be built in
    # a directory with whitespace in its name.
    if(${CMAKE_CURRENT_BINARY_DIR} MATCHES ".*[ \t].*")
      message(FATAL_ERROR
        "Can't build fftw in a directory with whitespace in its name")
    endif()
    #
    # build fftw as an external project
    if(BUILD_SHARED_LIBS)
      set(FFTW_SHARED_FLAG --enable-shared)
    endif()
    if(ITK_USE_FFTWF)

      # follow the standard EP_PREFIX locations
      set ( fftwf_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/fftwf/src/fftwf-build )
      set ( fftwf_source_dir ${CMAKE_CURRENT_BINARY_DIR}/fftwf/src/fftwf )

      configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/fftwf_configure_step.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/fftwf_configure_step.cmake
        @ONLY)

      set ( fftwf_CONFIGURE_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/fftwf_configure_step.cmake )

      ExternalProject_add(fftwf
        PREFIX fftwf
        URL "http://www.fftw.org/fftw-3.3.2.tar.gz"
        URL_MD5 6977ee770ed68c85698c7168ffa6e178
        CONFIGURE_COMMAND ${fftwf_CONFIGURE_COMMAND}
        )
    endif()

    if(ITK_USE_FFTWD)

      # follow the standard EP_PREFIX locations
      set ( fftwd_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/fftwd/src/fftwd-build )
      set ( fftwd_source_dir ${CMAKE_CURRENT_BINARY_DIR}/fftwd/src/fftwd )

      configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/fftwd_configure_step.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/fftwd_configure_step.cmake
        @ONLY)

      set ( fftwd_CONFIGURE_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/fftwd_configure_step.cmake )

      ExternalProject_add(fftwd
        PREFIX fftwd
        URL "http://www.fftw.org/fftw-3.3.2.tar.gz"
        URL_MD5 6977ee770ed68c85698c7168ffa6e178
        CONFIGURE_COMMAND  ${fftwd_CONFIGURE_COMMAND}
        )
    endif()
    set(FFTW_INCLUDE_PATH ${ITK_BINARY_DIR}/fftw/include)
    set(FFTW_LIBDIR ${ITK_BINARY_DIR}/fftw/lib)
    link_directories(${FFTW_LIBDIR})
    include_directories(${FFTW_INCLUDE_PATH})
    #
    # copy libraries into install tree
    install(CODE
      "file(GLOB FFTW_LIBS ${ITK_BINARY_DIR}/fftw/lib/*fftw3*)
file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/lib/ITK-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}\"
TYPE FILE FILES \${FFTW_LIBS})" COMPONENT Development)
    #
    # copy headers into install tree
    install(CODE
      "file(GLOB FFTW_INC ${ITK_BINARY_DIR}/fftw/include/*fftw3*)
file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/include/ITK-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}/Algorithms\"
TYPE FILE FILES \${FFTW_INC})" COMPONENT Development)

  endif()
endif()
