project(ITKVtkGlue)
set(ITKVtkGlue_LIBRARIES ITKVtkGlue)

#
# Add the third party includes and libraries
#

set(ITKVtkGlue_SYSTEM_INCLUDE_DIRS ${VTK_INCLUDE_DIRS})
set(ITKVtkGlue_SYSTEM_LIBRARY_DIRS ${VTK_LIBRARY_DIRS})

# Load to location of VTK libraries to avoid linking errors.
set(ITKVtkGlue_EXPORT_CODE_INSTALL "
set(_used_vtk_config \"${VTK_DIR}/VTKConfig.cmake\")
if(NOT DEFINED VTK_LIBRARIES AND EXISTS \"\${_used_vtk_config}\")
  include(\"\${_used_vtk_config}\")
endif()
")
set(ITKVtkGlue_EXPORT_CODE_BUILD "${ITKVtkGlue_EXPORT_CODE_INSTALL}")

itk_module_impl()
