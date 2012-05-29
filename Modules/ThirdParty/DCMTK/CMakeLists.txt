project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

option(ITK_USE_SYSTEM_DCMTK "Use an outside build of DCMTK." OFF)

# add to module path to find FindLIBICONV.cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake ${CMAKE_MODULE_PATH})

if(ITK_USE_SYSTEM_DCMTK)
  #
  find_package(DCMTK REQUIRED)
  # assign includes to std module var
  set(ITKDCMTK_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})
  #
  # accomodate a shortcoming in the FindDCMTK.cmake --
  # won't find headers with full path from include prefix
  # otherwise.
  list(APPEND ITKDCMTK_INCLUDE_DIRS ${DCMTK_DIR}/include)

  #
  # System DCMTK will be built depending on system iconv/jpeg/tiff
  # oflog is part of the DCMTK library but left out of the list
  # in FildDCMTK.cmake
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

  #
  # Module standard library var
  set(ITKDCMTK_LIBRARIES ${DCMTK_LIBRARIES})

else(ITK_USE_SYSTEM_DCMTK)

  # this is a first guess, I presume that
  # on windows the library name is dependent on
  # whether it's debug or release.
  #
  # also, .lib won't be right for shared libs.
  if(NOT WIN32)
    set(lib_prefix lib)
    set(lib_suffix .a)
  else(NOT WIN32)
    set(lib_prefix "")
    set(lib_suffix .lib)
  endif()

  # to do a proper build of DCMTK, it is necessary
  # to build current versions of several support libraries and link to them.
  set(PREREQS ${CMAKE_CURRENT_BINARY_DIR}/DCMTK_Prereqs)
  option(DCMTK_BUILD_OWN_LIBS "Build private copies of TIFF & JPEG" ON)
  #
  # Building versions of missing system libraries. Note that this is
  # redundant because ITK builds everything except ICONV
  if(DCMTK_BUILD_OWN_LIBS)
    find_package(JPEG QUIET)
    if(JPEG_FOUND)
      set(JPEG_ARGS
        -DJPEG_INCLUDE_DIRS:PATH=${JPEG_INCLUDE_DIRS}
        -DJPEG_INCLUDE_DIR:PATH=${JPEG_INCLUDE_DIRS}
        -DJPEG_LIBRARIES:STRING=${JPEG_LIBRARIES}
        -DJPEG_LIBRARY:FILEPATH=${JPEG_LIBRARY}
        )
      # message("JPEG LIBRARY FOUND: ${JPEG_LIBRARIES} ${JPEG_LIBRARY}")
    else()
      # message("Building JPEG")
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
      set(JPEG_DEPENDENCY libjpeg)
      set(JPEG_ARGS
        -DJPEG_INCLUDE_DIR:PATH=${PREREQS}/include
        -DJPEG_LIBRARY:FILEPATH=${PREREQS}/lib/${lib_prefix}jpeg${lib_suffix})
    endif()

    find_package(TIFF QUIET)
    if(TIFF_FOUND)
      set(TIFF_ARGS
        -DTIFF_INCLUDE_DIRS:PATH=${TIFF_INCLUDE_DIRS}
        -DTIFF_INCLUDE_DIR:PATH=${TIFF_INCLUDE_DIRS}
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
      set(TIFF_ARGS -DTIFF_INCLUDE_DIR:PATH=${PREREQS}/include
        -DTIFF_LIBRARY:FILEPATH=${PREREQS}/lib/${lib_prefix}tiff${lib_suffix})
    endif()
  else() # DCMTK_BUILD_OWN_LIBS
    #
    # right now this doesn't work at all
    # these variables come up blank.
    set(JPEG_ARGS
      -DJPEG_INCLUDE_DIR:PATH=${ITKJPEG_INCLUDE_DIRS}
      -DJPEG_LIBRARIES:PATH=${ITKJPEG_LIBRARIES}
      -DJPEG_LIBRARY:FILEPATH=${ITKJPEG_LIBRARIES}
      )
    set(TIFF_ARGS
      -DTIFF_INCLUDE_DIR:PATH=${ITKTIFF_INCLUDE_DIRS}
      -DTIFF_LIBRARIES:PATH=${ITKTIFF_LIBRARIES}
      -DTIFF_LIBRARY:PATH=${ITK_TIFF_LIBRARIES}
      )
    set(ZLIB_ARGS
      -DZLIB_INCLUDE_DIR:PATH=${ITKZLIB_INCLUDE_DIRS}
      -DZLIB_LIBRARIES:PATH=${ITKZLIB_LIBRARIES}
      -DZLIB_LIBRARY:PATH=${ITK_ZLIB_LIBRARIES}
      )
    message("JPEG_INCLUDE_DIR:PATH=${ITKJPEG_INCLUDE_DIRS}")
    message("JPEG_LIBRARIES:PATH=${ITKJPEG_LIBRARIES}")
    message("JPEG_LIBRARY:FILEPATH=${ITKJPEG_LIBRARIES}")
    message("TIFF_INCLUDE_DIR:PATH=${ITKTIFF_INCLUDE_DIRS}")
    message("TIFF_LIBRARIES:PATH=${ITKTIFF_LIBRARIES}")
    message("TIFF_LIBRARY:PATH=${ITK_TIFF_LIBRARIES}")
    message("ZLIB_INCLUDE_DIR:PATH=${ITKZLIB_INCLUDE_DIRS}")
    message("ZLIB_LIBRARIES:PATH=${ITKZLIB_LIBRARIES}")
    message("ZLIB_LIBRARY:PATH=${ITK_ZLIB_LIBRARIES}")
  endif() # DCMTK_BUILD_OWN_LIB

  # iconv library seems to be present unpredictably, almost always
  # on linux, sometimes on OS X and probably never on Windows
  find_package(LIBICONV)
  if(LIBICONV_FOUND)
    #message("ICONV LIBRARY FOUND: ${LIBICONV_LIBRARIES}")
    get_filename_component(LIBICONV_LIBDIR ${LIBICONV_LIBRARY} PATH)
    set(ICONV_ARGS
      -DLIBICONV_LIBDIR:PATH=${LIBICONV_LIBDIR}
      -DLIBICONV_INCLUDE_DIR:PATH=${LIBICONV_INCLUDE_DIRS}
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
    ${ZLIB_ARGS}
    ${ICONV_ARGS}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    DEPENDS ${JPEG_DEPENDENCY} ${TIFF_DEPENDENCY} ${ICONV_DEPENDENCY}
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
    iconv oflog)

endif(ITK_USE_SYSTEM_DCMTK)

#
# this won't be necessary if we can straighten out building against
# ITKZLIB
find_package(ZLIB REQUIRED)
foreach(lib ${ZLIB_LIBRARIES})
  list(APPEND ITKDCMTK_LIBRARIES ${lib})
endforeach(lib)

itk_module_impl()
