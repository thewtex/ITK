project(ItkVtkGlue)
include(CPack)


option(BUILD_WRAPPERS "Wrap library" OFF)
option(INSTALL_DEVEL_FILES "Install C++ headers" ON)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(BUILD_WRAPPERS)
   subdirs(Wrapping)
endif(BUILD_WRAPPERS)

# install devel files
if(INSTALL_DEVEL_FILES)
  foreach(f itkImageToVTKImageFilter.h itkImageToVTKImageFilter.txx itkVTKImageToImageFilter.h itkVTKImageToImageFilter.txx)
    install_files(/include/InsightToolkit/BasicFilters FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${f})
  endforeach(f)
endif(INSTALL_DEVEL_FILES)

enable_testing()
