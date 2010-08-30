# This CMakeLists.txt file handles the creation of files needed by
# other client projects that use VXL.  Nothing is built by this
# CMakeLists.txt file.  This CMakeLists.txt file must be processed by
# CMake after all the other CMakeLists.txt files in the VXL tree,
# which is why the subdirs(config/cmake/export) command is at the end
# of the top level CMakeLists.txt file.

# Needed to get non-cached variable settings used in VXLConfig.cmake.in
include( ${MODULE_PATH}/FindNetlib.cmake )
include( ${MODULE_PATH}/FindQv.cmake )
include( ${MODULE_PATH}/FindZLIB.cmake )
include( ${MODULE_PATH}/FindPNG.cmake )
include( ${MODULE_PATH}/FindJPEG.cmake )
include( ${MODULE_PATH}/FindTIFF.cmake )
include( ${MODULE_PATH}/FindGEOTIFF.cmake )
include( ${MODULE_PATH}/FindMPEG2.cmake )

# Save the compiler settings so another project can import them.
include(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
set(VXL_BUILD_SETTINGS_FILE ${vxl_BINARY_DIR}/VXLBuildSettings.cmake)
set(VXL_EXPORT_BUILD_SETTINGS_FILE
  ${CMAKE_INSTALL_PREFIX}/share/vxl/cmake/VXLBuildSettings.cmake
)
CMAKE_EXPORT_BUILD_SETTINGS(${VXL_BUILD_SETTINGS_FILE})

# Save library dependencies.
set(VXL_LIBRARY_DEPENDS_FILE ${vxl_BINARY_DIR}/VXLLibraryDepends.cmake)
set(VXL_INSTALL_LIBRARY_DEPENDS_FILE
  ${CMAKE_INSTALL_PREFIX}/share/vxl/cmake/VXLLibraryDepends.cmake
)
export_library_dependencies(${VXL_LIBRARY_DEPENDS_FILE})

# Create the VXLConfig.cmake file for the build tree.
configure_file(${VXL_CMAKE_DIR}/VXLConfig.cmake.in
               ${vxl_BINARY_DIR}/VXLConfig.cmake @ONLY IMMEDIATE)

configure_file(${VXL_CMAKE_DIR}/VXLConfig_export.cmake.in
               ${vxl_BINARY_DIR}/config/cmake/export/VXLConfig.cmake
               @ONLY IMMEDIATE)

install_files(/share/vxl/cmake FILES
  ${vxl_BINARY_DIR}/config/cmake/export/VXLConfig.cmake
  ${vxl_BINARY_DIR}/VXLBuildSettings.cmake
  ${vxl_BINARY_DIR}/VXLLibraryDepends.cmake
  ${VXL_CMAKE_DIR}/VXLStandardOptions.cmake
  ${VXL_CMAKE_DIR}/UseVXL.cmake
  ${VXL_CMAKE_DIR}/UseVGUI.cmake
)
