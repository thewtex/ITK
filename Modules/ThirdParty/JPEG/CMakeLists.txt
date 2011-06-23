project(ITK_JPEG)
set(ITK_JPEG_THIRD_PARTY 1)

option(ITK_USE_SYSTEM_JPEG "Use an outside build of JPEG." OFF)
mark_as_advanced(ITK_USE_SYSTEM_JPEG)

if(ITK_USE_SYSTEM_JPEG)
  find_package(JPEG REQUIRED)
  set(ITK_JPEG_INCLUDE_DIRS
    ${ITK_JPEG_BINARY_DIR}/src
    )
  set(ITK_JPEG_SYSTEM_INCLUDE_DIRS
    ${JPEG_INCLUDE_DIRS}
    )
  set(ITK_JPEG_LIBRARIES "${JPEG_LIBRARIES}")
  set(ITK_JPEG_NO_SRC 1)
else()
  set(ITK_JPEG_INCLUDE_DIRS
    ${ITK_JPEG_BINARY_DIR}/src
    ${ITK_JPEG_SOURCE_DIR}/src
    )
  set(ITK_JPEG_LIBRARIES itkjpeg)
endif()

configure_file(src/itk_jpeg.h.in src/itk_jpeg.h)

itk_module_impl()

install(FILES ${ITK_JPEG_BINARY_DIR}/src/itk_jpeg.h
  DESTINATION ${ITK_JPEG_INSTALL_INCLUDE_DIR})
