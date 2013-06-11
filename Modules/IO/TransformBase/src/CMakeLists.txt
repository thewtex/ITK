set(ITKIOTransformBase_SRC
itkTransformFactoryBase.cxx
)

add_library(ITKIOTransformBase ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOTransformBase_SRC})
target_link_libraries(ITKIOTransformBase ${ITKTransform_LIBRARIES})
itk_module_target(ITKIOTransformBase)
