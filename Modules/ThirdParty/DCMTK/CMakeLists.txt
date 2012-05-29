project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

if(NOT ITK_USE_SYSTEM_DCMTK)
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
    GIT_TAG 12690c81c05fbb0ec7087522bdc48dfea8aa528a
    SOURCE_DIR dcmtk
    BINARY_DIR dcmtk-build
    UPDATE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_LIST_DIR}/CMake/FindLIBICONV.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dcmtk/CMake/FindICONV.cmake
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DCMTK_PREREQS}
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



endif(NOT ITK_USE_SYSTEM_DCMTK)


itk_module_impl()
