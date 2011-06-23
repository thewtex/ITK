set(ITK_Transform_SRC
  itkHDF5TransformIO.cxx
  itkHDF5TransformIOFactory.cxx
  itkTxtTransformIO.cxx
  itkTxtTransformIOFactory.cxx
  itkTransformIOBase.cxx
  itkMatlabTransformIO.cxx
  itkTransformFactoryBase.cxx
  itkTransformFileWriter.cxx
  itkTransformFileReader.cxx
  itkMatlabTransformIOFactory.cxx
  itkTransformIOFactory.cxx
  )

add_library(ITK_Transform ${ITK_Transform_SRC})
target_link_libraries(ITK_Transform  ${ITK_Statistics_LIBRARIES} itkhdf5 itkhdf5_cpp)
itk_module_target(ITK_Transform)
