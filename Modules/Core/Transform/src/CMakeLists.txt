option(ITK_HD5_TRANSFORM_IO "Build HD5 Transform File I/O" ON)
if(ITK_HD5_TRANSFORM_IO)
  find_package(HDF5 REQUIRED NO_MODULE) # fcsv requires HDF5
  include_directories(${HDF5_INCLUDE_DIR_CPP} ${HDF5_INCLUDE_DIR})
  set(HD5_LIBS hdf5_cpp hdf5)
  set(HD5_Files
    itkHD5TransformIO.cxx
    itkHD5TransformIOFactory.cxx
    )
  set_property(SOURCE itkTransformIOFactory.cxx
    APPEND PROPERTY
    COMPILE_FLAGS "-DITK_HD5_TRANSFORM_IO")
endif(ITK_HD5_TRANSFORM_IO)

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
${HD5_Files}
)

add_library(ITK-Transform ${ITK-Transform_SRC})
target_link_libraries(ITK-Transform  ${ITK-ImageStatistics_LIBRARIES} ${TEST_DEPENDS_LIBRARIES} ${ITK-TestKernel_LIBRARIES} ${HD5_LIBS})
itk_module_target(ITK-Transform)
