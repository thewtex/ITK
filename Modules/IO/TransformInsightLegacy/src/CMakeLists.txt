set(ITKIOTransformInsightLegacy_SRC
itkTxtTransformIOFactory.cxx
)

add_library(ITKIOTransformInsightLegacy ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOTransformInsightLegacy_SRC})
target_link_libraries(ITKIOTransformInsightLegacy
  ${ITKIOTransformBase_LIBRARIES} ${ITKDoubleConversion_LIBRARIES})
itk_module_target(ITKIOTransformInsightLegacy)
