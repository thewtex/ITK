project(VideoIOVXL)

find_package ( VXL REQUIRED )
include( ${VXL_CMAKE_DIR}/UseVXL.cmake )

set(VideoIOVXL_INCLUDE_DIRS ${VXL_INCLUDE_DIRS} )
set(VideoIOVXL_LIBRARIES VideoIOVXL VideoCoreCommon vcl vidl avcodec avformat)
itk_module_impl()
