itk_wrap_module(ITKVtkGlue)
  set(WRAPPER_SWIG_LIBRARY_FILES ${WRAPPER_SWIG_LIBRARY_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/VtkGlue.i")
  set(WRAPPER_LIBRARY_DEPENDS ITKCommon
    ITKVTK)
  set(WRAPPER_LIBRARY_LINK_LIBRARIES ${ITK_LIBRARIES} ${PYTHON_LIBRARY} )

  itk_auto_load_submodules()
itk_end_wrap_module()

if(ITK_WRAP_PYTHON)
  set(VTK_VERSION "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.${VTK_BUILD_VERSION}")
  set(MIN_ITK_VER "5.0.0")
  if("${VTK_VERSION}" STRLESS "${MIN_ITK_VER}")
    target_link_libraries(ITKVtkGluePython vtkImagingPython)
  else("${VTK_VERSION}" STRLESS "${MIN_ITK_VER}")
    target_link_libraries(ITKVtkGluePython vtkImagingPythonD)
  endif("${VTK_VERSION}" STRLESS "${MIN_ITK_VER}")
endif(ITK_WRAP_PYTHON)
