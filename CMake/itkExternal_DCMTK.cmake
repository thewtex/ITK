
# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

# Set dependency list
set(DCMTK_DEPENDENCIES "")

# Include dependent projects if any
set(proj DCMTK)

if(NOT DEFINED DCMTK_DIR)
  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
  endif()

  set(DCMTK_REPOSITORY ${git_protocol}://git.dcmtk.org/dcmtk.git)
  set(DCMTK_GIT_TAG "6c07d41e5a7591f78e4bb988bcdcfba6b7a4967a")

  ExternalProject_Add(${proj}
    GIT_REPOSITORY ${DCMTK_REPOSITORY}
    GIT_TAG ${DCMTK_GIT_TAG}
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    INSTALL_DIR ${proj}-install
    "${cmakeversion_external_update}"
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DDCMTK_WITH_DOXYGEN:BOOL=OFF
      -DDCMTK_WITH_ZLIB:BOOL=OFF    # see CTK github issue #25
      -DDCMTK_WITH_OPENSSL:BOOL=OFF # see CTK github issue #25
      -DDCMTK_WITH_PNG:BOOL=OFF     # see CTK github issue #25
      -DDCMTK_WITH_TIFF:BOOL=OFF    # see CTK github issue #25
      -DDCMTK_WITH_XML:BOOL=OFF     # see CTK github issue #25
      -DDCMTK_WITH_ICONV:BOOL=OFF   # see CTK github issue #178
      -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
      -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
    DEPENDS
      ${DCMTK_DEPENDENCIES}
  )

  set(DCMTK_DIR ${CMAKE_BINARY_DIR}/${proj}-install)
  set(DCMTK_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/${proj}-install/include)
  include_directories(${DCMTK_INCLUDE_DIRS})
  set(DCMTK_LIBRARY_DIRECTORIES ${CMAKE_BINARY_DIR}/${proj}-install/lib)
  link_directories(${DCMTK_LIBRARY_DIRECTORIES})
  set(DCMTK_LIBNAMES charls dcmdata dcmdsig dcmimage dcmimgle dcmjpeg dcmjpls dcmnet dcmpstat dcmqrdb dcmrt dcmsr dcmtls dcmwlm i2d ijg12 ijg16 ijg8 oflog ofstd)
  foreach(libname ${DCMTK_LIBNAMES})
    list(APPEND DCMTK_LIBRARIES "${CMAKE_BINARY_DIR}/${proj}-install/lib/lib${libname}.a")
  endforeach()

  ## TODO:  Install libraries and headers with ITK (Look at FFTW implementation)
else()
  find_package(DCMTK Required)
  # The project is provided with DCMTK_DIR, nevertheless since other project may depend on DCMTK_DIR,
  # let's add an 'empty' one
  # SlicerMacroEmptyExternalProject(${proj} "${DCMTK_DEPENDENCIES}")
endif()
