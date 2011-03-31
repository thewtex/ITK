option(ITK_HDF5_IMAGE_IO "Build HDF5 Image File I/O" Off)
if(ITK_HDF5_IMAGE_IO)
  find_package(HDF5 REQUIRED NO_MODULE) # fcsv requires HDF5
  include_directories(${HDF5_INCLUDE_DIR_CPP} ${HDF5_INCLUDE_DIR})
  set(HDF5_LIBS hdf5_cpp hdf5)
endif(ITK_HDF5_IMAGE_IO)


set(ITK-IO-HDF5_SRC
itkHDF5ImageIOFactory.cxx
itkHDF5ImageIO.cxx
)

add_library(ITK-IO-HDF5 ${ITK-IO-HDF5_SRC})

target_link_libraries(ITK-IO-HDF5
${ITK-HDF5_LIBRARIES}
${ITK-IO-Base_LIBRARIES}
${TEST_DEPENDS_LIBRARIES}
${ITK-TestKernel_LIBRARIES}
${HDF5_LIBS}
)

itk_module_target(ITK-IO-HDF5)
