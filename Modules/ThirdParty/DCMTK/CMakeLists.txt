project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

#
# build DCMTK prereqs down in DCMTK dir
set(ITKDCMTK_BUILD_PREFIX
  ${ITK_BINARY_DIR}/Modules/ThirdParty/DCMTK)

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
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${ITKDCMTK_PREREQS}
    --enable-static=yes
    --enable-shared=no
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    )
  set(ICONV_ARGS
    -DLIBICONV_LIBDIR:PATH=${ITKDCMTK_PREREQS}/lib
    -DLIBICONV_INCLUDE_DIR:PATH=${ITKDCMTK_PREREQS}/include
    -DLIBICONV_LIBRARY:FILEPATH=${LIBICONV_LIBRARY})

  set(LIBICONV_LIBRARY ${ITKDCMTK_PREREQS}/lib/${lib_prefix}iconv${lib_suffix})
  set(ICONV_DEPENDENCY libiconv)
endif(LIBICONV_FOUND)

set_property(TARGET ITKDCMTK_iconv PROPERTY
  IMPORTED_LOCATION ${LIBICONV_LIBRARY})

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
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${ITKDCMTK_PREREQS}
    --enable-static=yes
    --enable-shared=no
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    )
  set(JPEG_DEPENDENCY libjpeg)
  set(JPEG_LIBRARY ${ITKDCMTK_PREREQS}/lib/${lib_prefix}jpeg${lib_suffix})
  set(JPEG_ARGS
    -DJPEG_INCLUDE_DIR:PATH=${ITKDCMTK_PREREQS}/include
    -DJPEG_LIBRARY:FILEPATH=${JPEG_LIBRARY})
endif()

#
# import the build jpeg target
set_property(TARGET ITKDCMTK_jpeg PROPERTY
  IMPORTED_LOCATION ${JPEG_LIBRARY})

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
    --prefix=${ITKDCMTK_PREREQS}
    --enable-shared=No
    --enable-static=Yes
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    DEPENDS libjpeg
    )
  # the FindTIFF.cmake is primitive, this doesn't help
  #    set(TIFF_ARGS -DTIFF_DIR:PATH=${ITKDCMTK_PREREQS})
  set(TIFF_LIBRARY ${ITKDCMTK_PREREQS}/lib/${lib_prefix}tiff${lib_suffix})
  set(TIFF_DEPENDENCY libtiff)
  set(TIFF_ARGS -DTIFF_INCLUDE_DIR:PATH=${ITKDCMTK_PREREQS}/include
    -DTIFF_LIBRARY:FILEPATH=${TIFF_LIBRARY})
endif()

set_property(TARGET ITKDCMTK_tiff PROPERTY
  IMPORTED_LOCATION ${TIFF_LIBRARY})

if(ITK_USE_SYSTEM_DCMTK)
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

else(ITK_USE_SYSTEM_DCMTK)

  option(DCMTK_BUILD_OWN_LIBS "Build private copies of TIFF & JPEG" ON)
  #
  # Building versions of missing system libraries. Note that this is
  # redundant because ITK builds everything except ICONV
  if(NOT DCMTK_BUILD_OWN_LIBS)
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



  #  GIT_REPOSITORY https://github.com/commontk/DCMTK.git
  ExternalProject_add(dcmtk
    GIT_REPOSITORY "git://git.dcmtk.org/dcmtk.git"
    GIT_TAG 228f0ef9d4064d1578d94cb7abf67cba71394193
    SOURCE_DIR dcmtk
    BINARY_DIR dcmtk-build
    UPDATE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_LIST_DIR}/CMake/FindLIBICONV.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dcmtk/CMake/FindICONV.cmake
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${ITKDCMTK_PREREQS}
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

  foreach(lib ${ITKDCMTK_LibNames})
    # give it a unique target name
    set(extlibname ITKDCMTK_${lib})
    # file name of library file
    set(libfilename ${ITKDCMTK_LIBDIR}/${lib_prefix}${lib}${lib_suffix})

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

  set(ITKDCMTK_INCLUDE_DIRS ${ITKDCMTK_PREREQS}/include)

  foreach(incdir ${DCMTK_INC})
    list(APPEND ITKDCMTK_INCLUDE_DIRS
      ${ITKDCMTK_PREREQS}/include/dcmtk/${incdir})
  endforeach(incdir)

  foreach(lib ${ITKDCMTK_LibNames})
    # give it a unique target name
    set(extlibname ITKDCMTK_${lib})

    # file name of library file
    set(libfilename ${ITKDCMTK_LIBDIR}/${lib_prefix}${lib}${lib_suffix})
    # tell the imported library where it's file lives
    set_property(TARGET ${extlibname} PROPERTY
      IMPORTED_LOCATION ${libfilename})
    # make the imported library depend on its prerequisite
    # libraries
    set_property(TARGET ${extlibname} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ITKDCMTK_jpeg ITKDCMTK_tiff ITKDCMTK_iconv
      ${ZLIB_LIBRARIES} )
    # make imported library target depend on external project target
    add_dependencies(${extlibname} dcmtk)

    # add to library list
    list(APPEND ITKDCMTK_LIBRARIES ${extlibname})
  endforeach()

endif(ITK_USE_SYSTEM_DCMTK)


itk_module_impl()
