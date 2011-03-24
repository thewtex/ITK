option(ITK_HDF5_TRANSFORM_IO "Build HDF5 Transform File I/O" ON)
if(ITK_HDF5_TRANSFORM_IO)
  find_package(HDF5 REQUIRED NO_MODULE) # fcsv requires HDF5
  include_directories(${HDF5_INCLUDE_DIR_CPP} ${HDF5_INCLUDE_DIR})
  set(HDF5_LIBS hdf5_cpp hdf5)
  set(HDF5_Files
    itkHDF5TransformIO.cxx
    itkHDF5TransformIOFactory.cxx
    )
  set_property(SOURCE itkTransformIOFactory.cxx
    APPEND PROPERTY
    COMPILE_FLAGS "-DITK_HDF5_TRANSFORM_IO")
endif(ITK_HDF5_TRANSFORM_IO)

set(ITK-Transform_SRC
itkTxtTransformIO.cxx
itkTxtTransformIOFactory.cxx
itkTransformIOBase.cxx
itkMatlabTransformIO.cxx
itkTransformFactoryBase.cxx
itkTransformFileWriter.cxx
itkTransformFileReader.cxx
itkMatlabTransformIOFactory.cxx
itkTransformIOFactory.cxx
${HDF5_Files}
)

add_library(ITK-Transform ${ITK-Transform_SRC})
target_link_libraries(ITK-Transform  ${ITK-ImageStatistics_LIBRARIES} ${TEST_DEPENDS_LIBRARIES} ${ITK-TestKernel_LIBRARIES} ${HDF5_LIBS})
itk_module_target(ITK-Transform)
