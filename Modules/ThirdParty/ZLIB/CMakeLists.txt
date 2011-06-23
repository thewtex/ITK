project(ITK_ZLIB)
set(ITK_ZLIB_THIRD_PARTY 1)

option(ITK_USE_SYSTEM_ZLIB "Use system-installed zlib" OFF)
mark_as_advanced(ITK_USE_SYSTEM_ZLIB)

if(ITK_USE_SYSTEM_ZLIB)
  find_package(ZLIB REQUIRED)
  set(ITK_ZLIB_INCLUDE_DIRS
    ${ITK_ZLIB_BINARY_DIR}/src
    )
  set(ITK_ZLIB_SYSTEM_INCLUDE_DIRS
    ${ZLIB_INCLUDE_DIRS}
    )
  set(ITK_ZLIB_LIBRARIES "${ZLIB_LIBRARIES}")
  set(ITK_ZLIB_NO_SRC 1)
else()
  set(ITK_ZLIB_INCLUDE_DIRS
    ${ITK_ZLIB_BINARY_DIR}/src
    ${ITK_ZLIB_SOURCE_DIR}/src
    )
  set(ITK_ZLIB_LIBRARIES itkzlib)
endif()

configure_file(src/itk_zlib.h.in src/itk_zlib.h)

itk_module_impl()

install(FILES ${ITK_ZLIB_BINARY_DIR}/src/itk_zlib.h
  DESTINATION ${ITK_ZLIB_INSTALL_INCLUDE_DIR})
