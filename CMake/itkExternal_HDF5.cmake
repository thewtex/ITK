if(ITK_USE_SYSTEM_HDF5)
  find_package(HDF5 REQUIRED NO_MODULE) # fcsv requires HDF5
  include_directories(${HDF5_INCLUDE_DIR_CPP} ${HDF5_INCLUDE_DIR})
  set(HDF5_LIBS hdf5_cpp hdf5)
else(ITK_USE_SYSTEM_HDF5)
  #
  # this patch of C Flags is necessary because at least
  # for OS X 10.6 w/gcc 4.2, there is an obscure link error
  # introduced if openmp is turned on.
  string(REPLACE "-fopenmp" "" HDF_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REPLACE "-fopenmp" "" HDF_CMAKE_CXX_FLAGS "${CMAKE_CX_FLAGS}")

  include(ExternalProject)

  set(HDF5_INSTALL_DIR ${ITK_BINARY_DIR}/HDF5)

  ExternalProject_add(HDF5
    SVN_REPOSITORY http://svn.hdfgroup.uiuc.edu/hdf5/branches/hdf5_1_8
    UPDATE_COMMAND ""
    CMAKE_ARGS
    # if you build debug you get libraries with _debug in their names
    # which is impossible to deal with. & it isn't like we're going
    # to be debugging HDF any time soon.
    --no-warn-unused-cli
    -DCMAKE_BUILD_TYPE:STRING=Release
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${HDF_CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${HDF_CMAKE_C_FLAGS}
    -DBUILD_SHARED_LIBS:BOOL=Off
    -DHDF5_BUILD_FORTRAN:BOOL=Off
    -DHDF5_ENABLE_Z_LIB_SUPPORT:BOOL=On
    -DHDF5_BUILD_CPP_LIB:BOOL=On
    #
    # if this is turned on, it will build h5dump etc for
    # you
    -DHDF5_BUILD_TOOLS:BOOL=Off
    -DCMAKE_INSTALL_PREFIX=${HDF5_INSTALL_DIR}
    INSTALL_DIR ${HDF5_INSTALL_DIR}
    )
  include_directories(${HDF5_INSTALL_DIR}/include
    ${HDF5_INSTALL_DIR}/include/cpp
    )
  link_directories(${ITK_BINARY_DIR}/HDF5/lib)
  set(HDF5_LIBRARIES hdf5_cpp hdf5)
  set(HDF5_DIR ${HDF5_INSTALL_DIR}/share/cmake/hdf5-1.8.8)
endif(ITK_USE_SYSTEM_HDF5)
