set(ITK_IO_HDF5_SRC
itkHDF5ImageIOFactory.cxx
itkHDF5ImageIO.cxx
)

add_library(ITK_IO_HDF5 ${ITK_IO_HDF5_SRC})

target_link_libraries(ITK_IO_HDF5
${ITK_HDF5_LIBRARIES}
${ITK_IO_Base_LIBRARIES}
${TEST_DEPENDS_LIBRARIES}
${ITK_TestKernel_LIBRARIES}
itkhdf5
itkhdf5_cpp
)

itk_module_target(ITK_IO_HDF5)
