project(ITK_HDF5)
set(ITK_HDF5_THIRD_PARTY 1)

option(ITK_USE_SYSTEM_HDF5 "Use an outside build of HDF5." OFF)
mark_as_advanced(ITK_USE_SYSTEM_HDF5)

if(ITK_USE_SYSTEM_HDF5)
  find_package(HDF5 REQUIRED C CXX)
  set(ITK_HDF5_SYSTEM_INCLUDE_DIRS
    ${ITK_HDF5_BINARY_DIR}/src # itk_hdf5.h and itk_H5Cpp.h
    ${HDF5_INCLUDE_DIRS}
    )
  set(ITK_HDF5_LIBRARIES ${HDF5_LIBRARIES})
  set(ITK_HDF5_NO_SRC 1)
else()
  set(ITK_HDF5_INCLUDE_DIRS
    ${ITK_HDF5_SOURCE_DIR}/src
    ${ITK_HDF5_BINARY_DIR}/src
    )
  set(ITK_HDF5_LIBRARIES itkhdf5_cpp itkhdf5)
endif()

configure_file(src/itk_hdf5.h.in src/itk_hdf5.h)
configure_file(src/itk_H5Cpp.h.in src/itk_H5Cpp.h)

itk_module_impl()

install(FILES ${ITK_HDF5_BINARY_DIR}/src/itk_hdf5.h
              ${ITK_HDF5_BINARY_DIR}/src/itk_H5Cpp.h
  DESTINATION ${ITK_HDF5_INSTALL_INCLUDE_DIR})
