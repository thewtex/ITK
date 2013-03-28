set(ITKIOTransformHDF5_SRC
itkHDF5TransformIOTemplate.cxx
itkHDF5TransformIOFactoryTemplate.cxx
)

add_library(ITKIOTransformHDF5 ${ITKIOTransformHDF5_SRC})
target_link_libraries(ITKIOTransformHDF5 ${ITKIOTransformBase_LIBRARIES} ${ITKHDF5_LIBRARIES})
itk_module_target(ITKIOTransformHDF5)
