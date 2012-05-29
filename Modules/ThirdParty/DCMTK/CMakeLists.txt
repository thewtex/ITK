project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)
set(ITKDCMTK_NO_SRC 1)

option(ITK_USE_SYSTEM_DCMTK "Use an outside build of DCMTK." OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake ${CMAKE_MODULE_PATH})

if(ITK_USE_SYSTEM_DCMTK)

  find_package(DCMTK REQUIRED)
  find_package(ZLIB REQUIRED)
  set(ITKDCMTK_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})
  list(APPEND ITKDCMTK_INCLUDE_DIRS ${DCMTK_DIR}/include)

  foreach(lib iconv jpeg tiff oflog)
    find_library(DCMTK_${lib}_LIBRARY
      ${lib}
      PATHS
      ${DCMTK_DIR}/${lib}/libsrc
      ${DCMTK_DIR}/${lib}/libsrc/Release
      ${DCMTK_DIR}/${lib}/libsrc/Debug
      ${DCMTK_DIR}/${lib}/Release
      ${DCMTK_DIR}/${lib}/Debug
      ${DCMTK_DIR}/lib)
    if(DCMTK_${lib}_LIBRARY)
      list(APPEND DCMTK_LIBRARIES ${DCMTK_${lib}_LIBRARY})
    endif()
  endforeach(lib)

  foreach(lib ${ZLIB_LIBRARIES})
    list(APPEND DCMTK_LIBRARIES ${lib})
  endforeach(lib)

  set(ITKDCMTK_LIBRARIES ${DCMTK_LIBRARIES})

else(ITK_USE_SYSTEM_DCMTK)

  # to do a proper build of DCMTK, it is necessary
  # to build current versions of several support libraries and link to them.
  set(PREREQS ${CMAKE_CURRENT_BINARY_DIR}/DCMTK_Prereqs)

  find_package(JPEG QUIET)
  if(JPEG_FOUND)
    set(JPEG_ARGS
      -DJPEG_INCLUDE_DIR:PATH=${JPEG_INCLUDE_DIR}
      -DJPEG_LIBRARIES:STRING=${JPEG_LIBRARIES}
      -DJPEG_LIBRARY:FILEPATH=${JPEG_LIBRARY}
      )
    message("JPEG LIBRARY FOUND: ${JPEG_LIBRARIES} ${JPEG_LIBRARY}")
  else()
    ExternalProject_add(libjpeg
      URL http://www.ijg.org/files/jpegsrc.v8d.tar.gz
      URL_MD5 52654eb3b2e60c35731ea8fc87f1bd29
      UPDATE_COMMAND ""
      CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREREQS}
      --enable-static=yes
      --enable-shared=no
      CC=${CMAKE_C_COMPILER}
      CXX=${CMAKE_CXX_COMPILER}
      )
    message("Building JPEG")
    # the FindTIFF.cmake is primitive, this doesn't help
    # set(JPEG_ARGS
    #   -DJPEG_DIR:PATH=${PREREQS})
    set(JPEG_DEPENDENCY libjpeg)
  endif()

  find_package(TIFF QUIET)
  if(TIFF_FOUND)
    set(TIFF_ARGS
      -DTIFF_INCLUDE_DIR:PATH=${TIFF_INCLUDE_DIR}
      -DTIFF_LIBRARIES:STRING=${TIFF_LIBRARIES}
      -DTIFF_LIBRARY:FILEPATH=${TIFF_LIBRARY}
      )
    message("TIFF LIBRARY FOUND: ${TIFF_LIBRARIES} ${TIFF_LIBRARY}")
  else()
    ExternalProject_add(libtiff
      URL ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/tiff-3.9.4.tar.gz
      URL_MD5 2006c1bdd12644dbf02956955175afd6
      CONFIGURE_COMMAND <SOURCE_DIR>/configure
      --prefix=${PREREQS} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
      --enable-shared=No
      --enable-static=Yes
      CC=${CMAKE_C_COMPILER}
      CXX=${CMAKE_CXX_COMPILER}
      DEPENDS libjpeg
      )
    message("Building TIFF")
    # the FindTIFF.cmake is primitive, this doesn't help
#    set(TIFF_ARGS -DTIFF_DIR:PATH=${PREREQS})
    set(TIFF_DEPENDENCY libtiff)
  endif()

  find_package(LIBICONV)
  if(LIBICONV_FOUND)
    message("ICONV LIBRARY FOUND: ${LIBICONV_LIBRARIES}")
    get_filename_component(LIBICONV_LIBDIR ${LIBICONV_LIBRARY} PATH)
    set(ICONV_ARGS
      -DLIBICONV_LIBDIR:PATH=${LIBICONV_LIBDIR}
      -DLIBICONV_INCLUDE_DIR:PATH=${LIBICONV_INCLUDE_DIR}
      -DLIBICONV_LIBRARY:PATH=${LIBICONV_LIBRARY}
      )
  else(LIBICONV_FOUND)
    ExternalProject_add(libiconv
      URL http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
      URL_MD5 e34509b1623cec449dfeb73d7ce9c6c6
      #  GIT_REPOSITORY git://git.savannah.gnu.org/libiconv.git
      UPDATE_COMMAND ""
      CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${PREREQS}
      --enable-static=yes
      --enable-shared=no
      CC=${CMAKE_C_COMPILER}
      CXX=${CMAKE_CXX_COMPILER}
      )
    message("Building ICONV")
    set(ICONV_ARGS
      -DLIBICONV_LIBDIR:PATH=${PREREQS}/lib
      -DLIBICONV_INCLUDE_DIR:PATH=${PREREQS}/include
      -DLIBICONV_LIBRARY:FILEPATH=${PREREQS}/lib/libiconv.a)
    set(ICONV_DEPENDENCY libiconv)
  endif(LIBICONV_FOUND)

  #  GIT_REPOSITORY https://github.com/commontk/DCMTK.git
  ExternalProject_add(dcmtk
    GIT_REPOSITORY "git://git.dcmtk.org/dcmtk.git"
    GIT_TAG 12690c81c05fbb0ec7087522bdc48dfea8aa528a
    SOURCE_DIR dcmtk
    BINARY_DIR dcmtk-build
    UPDATE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_LIST_DIR}/CMake/FindLIBICONV.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dcmtk/CMake/FindICONV.cmake
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PREREQS}
    ${COMMON_CMAKE_FLAGS}
    -DDCMTK_WITH_XML:BOOL=ON
    -DDCMTK_WITH_PRIVATE_TAGS:BOOL=ON
    ${JPEG_ARGS}
    ${TIFF_ARGS}
    ${ICONV_ARGS}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    DEPENDS ${TIFF_DEPENDENCY} ${JPEG_DEPENDENCY} ${ICONV_DEPENDENCY}
    )


  set(dcmtkPatchScript ${CMAKE_CURRENT_LIST_DIR}/CMake/dcmtkPatchScript.cmake)

  ExternalProject_Add_Step(dcmtk fixGenerateConfig
    COMMENT "Add JPEG library to dependencies"
    DEPENDEES download
    DEPENDERS configure
    COMMAND ${CMAKE_COMMAND}
    -Ddcmtk3rdParty=<SOURCE_DIR>/CMake/3rdparty.cmake
    -DdcmtkGenConfig=<SOURCE_DIR>/CMake/GenerateDCMTKConfigure.cmake
    -P ${dcmtkPatchScript}
    )

  #
  # set the important DCMTK variables
  set(DCMTK_INC config dcmdata dcmimage dcmimgle dcmjpeg dcmjpls dcmnet dcmpstat
    dcmqrdb dcmsign dcmsr dcmtls dcmwlm oflog ofstd)

  set(ITKDCMTK_INCLUDE_DIRS ${PREREQS}/include)

  foreach(incdir ${DCMTK_INC})
    list(APPEND ITKDCMTK_INCLUDE_DIRS ${PREREQS}/include/dcmtk/${incdir})
  endforeach(incdir)

  link_directories(${PREREQS}/lib)

  set(ITKDCMTK_LIBRARIES dcmdata dcmimage dcmimgle dcmjpeg
    dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 ofstd
    iconv jpeg tiff oflog z)

endif(ITK_USE_SYSTEM_DCMTK)

itk_module_impl()

if(TARGET dcmtk)
  add_dependencies(ITKDCMTK-all dcmtk)
endif()
