set(ITKIOTransformMatlab_SRC
itkMatlabTransformIO.cxx
itkMatlabTransformIOFactory.cxx
)

add_library(ITKIOTransformMatlab ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOTransformMatlab_SRC})
target_link_libraries(ITKIOTransformMatlab ${ITKIOTransformBase_LIBRARIES})
itk_module_target(ITKIOTransformMatlab)
