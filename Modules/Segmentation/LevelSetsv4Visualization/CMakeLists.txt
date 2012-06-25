project(ITKLevelSetsv4Visualization)

# VTK 5.8 and greater defines VTK_LIBRARIES
if(NOT VTK_LIBRARIES)
  set(VTK_LIBRARIES_EXPLICIT vtkRendering vtkHybrid)
endif(NOT VTK_LIBRARIES)

string(REGEX REPLACE "[^;]*vtkmetaio;"
         "" VTK_INCLUDE_DIRS "${VTK_INCLUDE_DIRS}")
string(REGEX REPLACE "[^;]*DICOMParser;"
         "" VTK_INCLUDE_DIRS "${VTK_INCLUDE_DIRS}")
set(ITKLevelSetsv4Visualization_LIBRARIES ${VTK_LIBRARIES}
  ${VTK_LIBRARIES_EXPLICIT} ITKVtkGlue )

itk_module_impl()
