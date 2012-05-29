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

set(ITKDCMTK_PREREQS
  ${ITKDCMTK_BINARY_DIR}/DCMTK_Prereqs)

# iconv library seems to be present unpredictably, never
# on linux, sometimes on OS X and probably never on Windows
find_package(LIBICONV QUIET)
add_library(ITKDCMTK_iconv STATIC IMPORTED)


if(NOT ITK_USE_SYSTEM_DCMTK)

  set(DCMTK_EPNAME ITKDCMTK_ExtProject)

  set(ITKDCMTK_LIBDIR ${ITKDCMTK_PREREQS}/lib)

  set(ITKDCMTK_LibNames dcmdata dcmimage dcmimgle dcmjpeg
    dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)

  set(ITKDCMTK_LIBRARIES "")

  foreach(lib ${ITKDCMTK_LibNames})
    # give it a unique target name
    set(extlibname ITKDCMTK_${lib})
    # add it as a library target
    add_library(${extlibname} STATIC IMPORTED)
    list(APPEND ITKDCMTK_LIBRARIES ${extlibname})
  endforeach()
endif(NOT ITK_USE_SYSTEM_DCMTK)
