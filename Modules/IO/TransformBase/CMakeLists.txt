project(ITKIOTransformBase)
set(ITKIOTransformBase_LIBRARIES ITKIOTransformBase)

set(
  ITK_TRANSFORM_FACTORY_MAX_DIM
  4
  CACHE STRING
  "The transform dimensions in the range [2,(this number)] supported by the TransformIO."
)
mark_as_advanced(ITK_TRANSFORM_FACTORY_MAX_DIM)
if(ITK_TRANSFORM_FACTORY_MAX_DIM LESS "3")
  message(
    FATAL_ERROR
    "ITK_TRANSFORM_FACTORY_MAX_DIM must be an integer greater than 2."
  )
endif()

add_definitions(
  "-DITK_TRANSFORM_FACTORY_MAX_DIM=${ITK_TRANSFORM_FACTORY_MAX_DIM}"
)

itk_module_impl()
