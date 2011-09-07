project(VideoIOOpenCV)

find_package ( OpenCV REQUIRED )

set(VideoIOOpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} )
set(VideoIOOpenCV_LIBRARIES ${OpenCV_LIBS} VideoCoreCommon VideoIOOpenCV)
itk_module_impl()
