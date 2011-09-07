project(VideoCoreOpenCV)

find_package ( OpenCV REQUIRED )

set( VideoCoreOpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} )
itk_module_impl()
