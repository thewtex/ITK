project(ITK_TIFF)
set(ITK_TIFF_THIRD_PARTY 1)

option(ITK_USE_SYSTEM_TIFF "Use system-installed tiff" OFF)
mark_as_advanced(ITK_USE_SYSTEM_TIFF)

if(ITK_USE_SYSTEM_TIFF)
  find_package(TIFF REQUIRED)
  set(ITK_TIFF_INCLUDE_DIRS
    ${ITK_TIFF_BINARY_DIR}/src
    )
  set(ITK_TIFF_SYSTEM_INCLUDE_DIRS
    ${TIFF_INCLUDE_DIRS}
    )
  set(ITK_TIFF_LIBRARIES "${TIFF_LIBRARIES}")
  set(ITK_TIFF_NO_SRC 1)
else()
  set(ITK_TIFF_INCLUDE_DIRS
    ${ITK_TIFF_BINARY_DIR}/src
    ${ITK_TIFF_SOURCE_DIR}/src
    )
  set(ITK_TIFF_LIBRARIES itktiff)
endif()

configure_file(src/itk_tiff.h.in src/itk_tiff.h)

itk_module_impl()

install(FILES ${ITK_TIFF_BINARY_DIR}/src/itk_tiff.h
  DESTINATION ${ITK_TIFF_INSTALL_INCLUDE_DIR})
