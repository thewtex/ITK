project(ITKNrrdIO)
include_regular_expression("^.*.h$")

#
# This CMake file configures the NrrdIO library build.  NrrdIO
# is used by Insight/Code/IO/itkNrrdIO for reading/writing
# "Nearly Raw Raster Data" within the open-source Teem software
# package. See http://teem.sourceforge.net for more information.
#

set(nrrdio_SRCS 754.c mop.c array.c parseAir.c dio.c sane.c endianAir.c
string.c enum.c miscAir.c biffbiff.c biffmsg.c accessors.c defaultsNrrd.c
enumsNrrd.c arraysNrrd.c methodsNrrd.c reorder.c axis.c simple.c comment.c
keyvalue.c endianNrrd.c parseNrrd.c gzio.c read.c write.c format.c
formatNRRD.c encoding.c encodingRaw.c encodingAscii.c encodingHex.c
encodingGzip.c subset.c encodingBzip2.c formatEPS.c formatPNG.c
formatPNM.c formatText.c formatVTK.c )

# Turn on TEEM_BUILD so that the proper dll export def's are
# used on windows builds.
add_definitions(-DTEEM_BUILD=1)

# Configure files with settings for use by the build.
configure_file(${ITKNrrdIO_SOURCE_DIR}/NrrdConfigure.h.in
  ${ITKNrrdIO_BINARY_DIR}/NrrdConfigure.h)

# Need to be able to fine NrrdConfig.h
include_directories(
        ${ITKNrrdIO_BINARY_DIR}
)

# Set compiler flags for endian-ness.
#if(CMAKE_WORDS_BIGENDIAN)
#  add_definitions(-DTEEM_ENDIAN=4321)
#else(CMAKE_WORDS_BIGENDIAN)
#  add_definitions(-DTEEM_ENDIAN=1234)
#endif(CMAKE_WORDS_BIGENDIAN)

#The QNANHIBIT variable is configured by the root level CMakeLists.txt
if(QNANHIBIT)
  add_definitions(-DTEEM_QNANHIBIT=1)
else(QNANHIBIT)
  add_definitions(-DTEEM_QNANHIBIT=0)
endif(QNANHIBIT)

#DirectIO is the fast way to do multi-gigabyte I/O and currently only available
#for SGI platforms.  Use of DirectIO is enabled manually for now.
#option(USE_DIRECTIO "Use DirectIO for Nrrd file IO.  Only valid on SGI systems." 0)
#mark_as_advanced(USE_DIRECTIO)
#if(USE_DIRECTIO)
#  add_definitions(-DTEEM_DIO=1)
#else(USE_DIRECTIO)
  add_definitions(-DTEEM_DIO=0)
#endif(USE_DIRECTIO)

# Tell NRRD to enable nrrdEncodingGzip
add_definitions(-DTEEM_ZLIB=1)

add_library(ITKNrrdIO ${nrrdio_SRCS} )
target_link_libraries(ITKNrrdIO ${ITK_ZLIB_LIBRARIES} )

if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(ITKNrrdIO PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif(ITK_LIBRARY_PROPERTIES)

if(NOT ITK_INSTALL_NO_LIBRARIES)
  install(TARGETS ITKNrrdIO
    RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT Development
    )
endif(NOT ITK_INSTALL_NO_LIBRARIES)

if(ITK_USE_FLAT_DIRECTORY_INSTALL)
  set( ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_NRRDIO ${ITK_INSTALL_INCLUDE_DIR_CM24})
else(ITK_USE_FLAT_DIRECTORY_INSTALL)
  set( ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_NRRDIO ${ITK_INSTALL_INCLUDE_DIR_CM24}/Utilities/NrrdIO)
endif(ITK_USE_FLAT_DIRECTORY_INSTALL)


if(NOT ITK_INSTALL_NO_DEVELOPMENT)
  file(GLOB __files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  install(FILES ${__files}
    DESTINATION ${ITK_INSTALL_INCLUDE_DIR_CM24_UTILITIES_NRRDIO}
    COMPONENT Development)
endif(NOT ITK_INSTALL_NO_DEVELOPMENT)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
endif(CMAKE_COMPILER_IS_GNUCXX)
