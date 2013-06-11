set(ITKIOTransformHDF5_SRC
itkHDF5TransformIO.cxx
itkHDF5TransformIOFactory.cxx
)

add_library(ITKIOTransformHDF5 ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOTransformHDF5_SRC})
target_link_libraries(ITKIOTransformHDF5 ${ITKIOTransformBase_LIBRARIES} ${ITKHDF5_LIBRARIES})
itk_module_target(ITKIOTransformHDF5)
