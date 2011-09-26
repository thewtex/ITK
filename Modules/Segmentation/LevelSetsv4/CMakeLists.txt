project(ITKLevelSetsv4)

option( LevelSetsv4_Visualization "Specific classes for level-sets visualization" OFF )

if( LevelSetsv4_Visualization )
  find_package( VTK REQUIRED )
  add_subdirectory( Visualization )
endif()

itk_module_impl()
