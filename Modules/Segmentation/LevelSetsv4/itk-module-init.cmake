option( LevelSetsv4_VIZ "Build visualization for the level sets" OFF )

if( LevelSetsv4_VIZ )
  find_package(VTK REQUIRED)
  if(VTK_FOUND)
    include(${VTK_USE_FILE})
  endif()
endif()
