option(ITK_USE_SYSTEM_DCMTK "Use an outside build of DCMTK." OFF)

# add to module path to find FindLIBICONV.cmake
set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_LIST_DIR}/CMake ${CMAKE_MODULE_PATH})

# this is a first guess, I presume that
# on windows the library name is dependent on
# whether it's debug or release.
#
# also, .lib won't be right for shared libs.
if(NOT WIN32 OR MINGW)
  set(lib_prefix lib)
  set(lib_suffix .a)
else(NOT WIN32)
  set(lib_prefix "")
  set(lib_suffix .lib)
endif()

# to do a proper build of DCMTK, it is necessary
# to build current versions of several support libraries and link to them.
set(DCMTK_PREREQS
  ${ITK_BINARY_DIR}/Modules/ThirdParty/DCMTK/DCMTK_Prereqs)

#
# build DCMTK prereqs down in DCMTK dir
set(_DCMTK_BUILD_PREFIX
  ${ITK_BINARY_DIR}/Modules/ThirdParty/DCMTK)
#
# this won't be necessary if we can straighten out building against
# ITKZLIB
find_package(ZLIB REQUIRED)

# iconv library seems to be present unpredictably, never
# on linux, sometimes on OS X and probably never on Windows
find_package(LIBICONV QUIET)
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
    PREFIX ${_DCMTK_BUILD_PREFIX}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DCMTK_PREREQS}
    --enable-static=yes
    --enable-shared=no
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    )
  set(ICONV_ARGS
    -DLIBICONV_LIBDIR:PATH=${DCMTK_PREREQS}/lib
    -DLIBICONV_INCLUDE_DIR:PATH=${DCMTK_PREREQS}/include
    -DLIBICONV_LIBRARY:FILEPATH=${LIBICONV_LIBRARY})

  set(LIBICONV_LIBRARY ${DCMTK_PREREQS}/lib/${lib_prefix}iconv${lib_suffix})
  set(ICONV_DEPENDENCY libiconv)
endif(LIBICONV_FOUND)

add_library(ITKDCMTK_iconv STATIC IMPORTED)
set_property(TARGET ITKDCMTK_iconv PROPERTY
  IMPORTED_LOCATION ${LIBICONV_LIBRARY})

find_package(JPEG QUIET)
if(JPEG_FOUND)
  get_filename_component(JPEG_DIR ${JPEG_LIBRARY} PATH)
  set(JPEG_ARGS
    -DJPEG_DIR:PATH=${JPEG_DIR}
    -DJPEG_INCLUDE_DIR:PATH=${JPEG_INCLUDE_DIR}
    )
  # message("JPEG LIBRARY FOUND: ${JPEG_LIBRARIES} ${JPEG_LIBRARY}")
else()
  # message("Building JPEG")
  ExternalProject_add(libjpeg
    URL http://www.ijg.org/files/jpegsrc.v8d.tar.gz
    URL_MD5 52654eb3b2e60c35731ea8fc87f1bd29
    UPDATE_COMMAND ""
    PREFIX ${_DCMTK_BUILD_PREFIX}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DCMTK_PREREQS}
    --enable-static=yes
    --enable-shared=no
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    )
  set(JPEG_DEPENDENCY libjpeg)
  set(JPEG_LIBRARY ${DCMTK_PREREQS}/lib/${lib_prefix}jpeg${lib_suffix})
  set(JPEG_ARGS
    -DJPEG_INCLUDE_DIR:PATH=${DCMTK_PREREQS}/include
    -DJPEG_LIBRARY:FILEPATH=${JPEG_LIBRARY})
endif()
#
# import the build jpeg target
add_library(ITKDCMTK_jpeg STATIC IMPORTED)
set_property(TARGET ITKDCMTK_jpeg PROPERTY
  IMPORTED_LOCATION ${JPEG_LIBRARY})

find_package(TIFF QUIET)
if(TIFF_FOUND)
  get_filename_component(TIFF_DIR ${TIFF_LIBRARY} PATH)
  set(TIFF_ARGS
    -DTIFF_DIR:PATH=${TIFF_DIR}
    -DTIFF_INCLUDE_DIR:PATH=${TIFF_INCLUDE_DIR}
    )
else()
  ExternalProject_add(libtiff
    URL ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/support/tiff-3.9.4.tar.gz
    URL_MD5 2006c1bdd12644dbf02956955175afd6
    PREFIX ${_DCMTK_BUILD_PREFIX}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure
    --prefix=${DCMTK_PREREQS}
    --enable-shared=No
    --enable-static=Yes
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    DEPENDS libjpeg
    )
  # the FindTIFF.cmake is primitive, this doesn't help
  #    set(TIFF_ARGS -DTIFF_DIR:PATH=${DCMTK_PREREQS})
  set(TIFF_LIBRARY ${DCMTK_PREREQS}/lib/${lib_prefix}tiff${lib_suffix})
  set(TIFF_DEPENDENCY libtiff)
  set(TIFF_ARGS -DTIFF_INCLUDE_DIR:PATH=${DCMTK_PREREQS}/include
    -DTIFF_LIBRARY:FILEPATH=${TIFF_LIBRARY})
endif()

add_library(ITKDCMTK_tiff STATIC IMPORTED)
set_property(TARGET ITKDCMTK_tiff PROPERTY
  IMPORTED_LOCATION ${TIFF_LIBRARY})

if(NOT ITK_USE_SYSTEM_DCMTK)

  set(DCMTK_LIBDIR ${DCMTK_PREREQS}/lib)

  set(libnames dcmdata dcmimage dcmimgle dcmjpeg
    dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
  set(ITKDCMTK_LIBRARIES "")

  foreach(lib ${libnames})
    # give it a unique target name
    set(extlibname ITKDCMTK_${lib})
    # add it as a library target
    add_library(${extlibname} STATIC IMPORTED)
    # file name of library file
    set(libfilename ${DCMTK_LIBDIR}/${lib_prefix}${lib}${lib_suffix})

    set_property(TARGET ${extlibname} PROPERTY
      IMPORTED_LOCATION ${libfilename})

    set_property(TARGET ${extlibname} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ITKDCMTK_jpeg ITKDCMTK_tiff ITKDCMTK_iconv
      ${ZLIB_LIBRARIES} )
    list(APPEND ITKDCMTK_LIBRARIES ${extlibname})
  endforeach()

  set(DCMTK_INC config dcmdata dcmimage dcmimgle
    dcmjpeg dcmjpls dcmnet dcmpstat
    dcmqrdb dcmsign dcmsr dcmtls dcmwlm oflog ofstd)

  set(ITKDCMTK_INCLUDE_DIRS ${DCMTK_PREREQS}/include)

  foreach(incdir ${DCMTK_INC})
    list(APPEND ITKDCMTK_INCLUDE_DIRS
      ${DCMTK_PREREQS}/include/dcmtk/${incdir})
  endforeach(incdir)
else(ITK_USE_SYSTEM_DCMTK)
  #
  find_package(DCMTK REQUIRED)
  # assign includes to std module var
  set(ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})
  #
  # accomodate a shortcoming in the FindDCMTK.cmake --
  # won't find headers with full path from include prefix
  # otherwise.
  list(APPEND ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_DIR}/include)

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
  set(ITKDCMTK_SYSTEM_LIBRARIES ${DCMTK_LIBRARIES})

endif(NOT ITK_USE_SYSTEM_DCMTK)
