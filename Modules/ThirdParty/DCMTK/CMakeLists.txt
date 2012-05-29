project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

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
    PREFIX ${ITKDCMTK_BINARY_DIR}
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


#
# if DCMTK is built elsewhere
if(ITK_USE_SYSTEM_DCMTK)

  find_package(DCMTK REQUIRED)

  # assign includes to std module var
  set(ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})

  # Module standard library var
  set(ITKDCMTK_LIBRARIES ${DCMTK_LIBRARIES})

  itk_module_impl()

else(ITK_USE_SYSTEM_DCMTK)
  #

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

  foreach(libdep JPEG TIFF ZLIB)
    #
    # if we're using ITK-built versions of libraries, then
    # have to find the actual library name, instead of the cmake logical name
    set(DCMTK${libdep}_LIBRARIES "")

    foreach(_lib ${ITK${libdep}_LIBRARIES})
      if(TARGET ${_lib})
        set(_lib $<TARGET_FILE:${_lib}>)
      endif()
      list(APPEND DCMTK${libdep}_LIBRARIES ${_lib})
    endforeach()

    # have to replace ; with another separator in order to pass lists into
    # the external project without them getting messed up.
    string(REPLACE ";" ":::" DCMTK${libdep}_LIBRARIES "${DCMTK${libdep}_LIBRARIES}")
    string(REPLACE ";" ":::" DCMTK${libdep}_INCLUDE_DIRS "${ITK${libdep}_INCLUDE_DIRS}")
  endforeach()
  #
  # DCMTK-RT is a branch on GIT that supports the RT
  # Extensions. This is the version that Slicer will track.
  # by and large it won't affect how DCMTK is used by ITK, but
  # it will at least be consistent in Slicer, where ITK will
  # presumably be built with ITK_USE_SYSTEM_DCMTK
  if(NOT UseDCMTKRT)
    set(DCMTK_GIT_REPOSITORY "git://git.dcmtk.org/dcmtk.git")
    set(DCMTK_GIT_TAG GIT_TAG 228f0ef9d4064d1578d94cb7abf67cba71394193)
  else()
    set(DCMTK_GIT_REPOSITORY "http://git.dcmtk.org/dcmtk.git.rt")
    set(DCMTK_GIT_TAG "f2b71c92461085718f3bccd1c7415382d3e11e3a")
  endif()

  ExternalProject_add(${DCMTK_EPNAME}
    GIT_REPOSITORY ${DCMTK_GIT_REPOSITORY}
    GIT_TAG ${DCMTK_GIT_TAG}
    SOURCE_DIR ${DCMTK_EPNAME}
    BINARY_DIR ${DCMTK_EPNAME}-build
    UPDATE_COMMAND ""
    LIST_SEPARATOR ":::"
    PATCH_COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_LIST_DIR}/CMake/FindLIBICONV.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME}/CMake/FindICONV.cmake
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${ITKDCMTK_PREREQS}
    ${COMMON_CMAKE_FLAGS}
    -DDCMTK_WITH_XML:BOOL=ON
    -DDCMTK_WITH_PRIVATE_TAGS:BOOL=ON
    -DJPEG_INCLUDE_DIR:STRING=${DCMTKJPEG_INCLUDE_DIRS}
    -DJPEG_LIBRARY:PATH=${DCMTKJPEG_LIBRARIES}
    -DTIFF_INCLUDE_DIR:STRING=${DCMTKTIFF_INCLUDE_DIRS}
    -DTIFF_LIBRARY:PATH=${DCMTKTIFF_LIBRARIES}
    -DZLIB_INCLUDE_DIR:STRING=${DCMTKZLIB_INCLUDE_DIRS}
    -DZLIB_LIBRARY:PATH=${DCMTKZLIB_LIBRARIES}
    ${ICONV_ARGS}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    DEPENDS ${JPEG_DEPENDENCY} ${TIFF_DEPENDENCY} ${ICONV_DEPENDENCY}
    )


  set(dcmtkPatchScript ${CMAKE_CURRENT_LIST_DIR}/CMake/dcmtkPatchScript.cmake)

  ExternalProject_Add_Step(${DCMTK_EPNAME} fixGenerateConfig
    COMMENT "Add JPEG library to dependencies"
    DEPENDEES download
    DEPENDERS configure
    COMMAND ${CMAKE_COMMAND}
    -DdcmtkSource=<SOURCE_DIR>
    -DITK_USE_SYSTEM_ZLIB=${ITK_USE_SYSTEM_ZLIB}
    -DITK_USE_SYSTEM_TIFF=${ITK_USE_SYSTEM_TIFF}
    -P ${dcmtkPatchScript}
    )

  # build the list of libraries upon which DCMTK depends
  set(ITKDCMTK_LIBDEP "")
  foreach(lib ITKJPEG_LIBRARIES ITKTIFF_LIBRARIES
      ITKZLIB_LIBRARIES ITKDCMTK_iconv)
    foreach(_lib ${lib})
      if(TARGET ${_lib})
        list(APPEND ITKDCMTK_LIBDEP ${_lib})
      endif()
    endforeach()
  endforeach()

  # attach actual filenames to the
  # imported libraries from the ExternalProject
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
      ${ITKDCMTK_LIBDEP} )
    # make imported library target depend on external project target
    add_dependencies(${extlibname} ${DCMTK_EPNAME})

  endforeach()

endif(ITK_USE_SYSTEM_DCMTK)
