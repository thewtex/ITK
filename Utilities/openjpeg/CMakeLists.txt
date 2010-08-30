project(ITKOpenJPEG C)
include_regular_expression("^.*$")

# Defines the source code for the library
set(OpenJPEG_SRCS
  tcd.c
  bio.c
  cio.c
  dwt.c
  event.c
  image.c
  j2k.c
  j2k_lib.c
  jp2.c
  jpt.c
  mct.c
  mqc.c
  openjpeg.c
  pi.c
  raw.c
  t1.c
  tgt.c
  profile.c
  invert.c
  function_list.c
  t2.c
  bio.h
  cio.h
  dwt.h
  event.h
  image.h
  j2k.h
  j2k_lib.h
  jp2.h
  jpt.h
  mct.h
  mqc.h
  openjpeg.h
  pi.h
  int.h
  raw.h
  t1.h
  t2.h
  tcd.h
  tgt.h
  profile.h
  invert.h
  function_list.h
)

configure_file(${ITKOpenJPEG_SOURCE_DIR}/.NoDartCoverage
               ${ITKOpenJPEG_BINARY_DIR}/.NoDartCoverage)


# Pass proper definition to preprocessor to generate shared lib
if(WIN32)
  if(BUILD_SHARED_LIBS)
    add_definitions(-DOPJ_EXPORTS)
  else(BUILD_SHARED_LIBS)
    add_definitions(-DOPJ_STATIC)
  endif(BUILD_SHARED_LIBS)
endif(WIN32)

# Create the library
add_library(${OPJ_PREFIX}openjpeg ${OpenJPEG_SRCS})

if(UNIX)
  target_link_libraries(${OPENJPEG_LIBRARY_NAME} m)
endif(UNIX)

# Install library
if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(${OPJ_PREFIX}openjpeg PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif(ITK_LIBRARY_PROPERTIES)

if(NOT ITK_INSTALL_NO_LIBRARIES)
  install(TARGETS ${OPJ_PREFIX}openjpeg
    EXPORT ITKTargets
    RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
endif(NOT ITK_INSTALL_NO_LIBRARIES)
