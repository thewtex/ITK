set(ITK_BiasCorrection_SRC
itkCompositeValleyFunction.cxx
itkCacheableScalarFunction.cxx
)

add_library(ITK_BiasCorrection ${ITK_BiasCorrection_SRC})
target_link_libraries(ITK_BiasCorrection  ${ITK_ImageFilterBase_LIBRARIES} ${ITK_Polynomials_LIBRARIES} ${ITK_Statistics_LIBRARIES} ${ITK_Optimizers_LIBRARIES})
itk_module_target(ITK_BiasCorrection)
