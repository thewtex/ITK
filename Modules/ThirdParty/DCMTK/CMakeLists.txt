project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

#
# if DCMTK is built elsewhere
if(ITK_USE_SYSTEM_DCMTK)
  find_package(TIFF REQUIRED)
  find_package(JPEG REQUIRED)
  find_pacakage(ZLIB REQUIRED)

  find_package(DCMTK REQUIRED)
  # assign includes to std module var
  set(ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})
  #
  # accomodate a shortcoming in the FindDCMTK.cmake --
  # won't find headers with full path from include prefix
  # otherwise.
  list(APPEND ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_DIR}/include)

  #
  # Module standard library var
  set(ITKDCMTK_LIBRARIES ${DCMTK_LIBRARIES})

  itk_module_impl()

else(ITK_USE_SYSTEM_DCMTK)
  #
  # build DCMTK prereqs down in DCMTK dir
  set(ITKDCMTK_BUILD_PREFIX
    ${ITK_BINARY_DIR}/Modules/ThirdParty/DCMTK)

  set(ITKDCMTK_INCLUDE ${ITKDCMTK_PREREQS}/include)

  set(ITKDCMTK_INCLUDE_DIRS ${ITKDCMTK_INCLUDE})

  set(ITKDCMTK_Include_Names ${ITKDCMTK_LibNames})
  # config is an include dir, but has no corresponding
  # library
  list(APPEND ITKDCMTK_Include_Names config)

  foreach(lib ${ITKDCMTK_Include_Names})
    # add to include list
    list(APPEND ITKDCMTK_INCLUDE_DIRS
      ${ITKDCMTK_INCLUDE}/dcmtk/${lib})
  endforeach()

  # implement module before the ExternalProject, to process
  # dependencies
  itk_module_impl()

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
    -DTIFF_INCLUDE_DIR:PATH=${ITKTIFF_INCLUDE_DIRS}
    -DTIFF_LIBRARY:PATH=$<TARGET_FILE:${ITKTIFF_LIBRARIES}>
    -DZLIB_INCLUDE_DIRS:STRING=${ITKZLIB_INCLUDE_DIRS}
    -DZLIB_LIBRARY:PATH=$<TARGET_FILE:${ITKZLIB_LIBRARIES}>
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
    -DdcmtkSource=<SOURCE_DIR>
    -P ${dcmtkPatchScript}
    )

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

  endforeach()

endif(ITK_USE_SYSTEM_DCMTK)
