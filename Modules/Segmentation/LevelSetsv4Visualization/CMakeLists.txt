project(ITKLevelSetsv4Visualization)

# VTK 5.8 and greater defines VTK_LIBRARIES
if(NOT VTK_LIBRARIES)
  set(VTK_LIBRARIES_EXPLICIT vtkRendering vtkHybrid)
endif()

set(ITKLevelSetsv4Visualization_LIBRARIES ${VTK_LIBRARIES}
  ${VTK_LIBRARIES_EXPLICIT} ITKVtkGlue )

# Load to location of VTK libraries to avoid linking errors.
set(ITKVtkGlue_EXPORT_CODE_INSTALL "
set(_used_vtk_config \"${VTK_DIR}/VTKConfig.cmake\")
if(NOT DEFINED VTK_LIBRARIES AND EXISTS \"\${_used_vtk_config}\")
  include(\"\${_used_vtk_config}\")
endif()
")
set(ITKVtkGlue_EXPORT_CODE_BUILD "${ITKVtkGlue_EXPORT_CODE_INSTALL}")

itk_module_impl()
