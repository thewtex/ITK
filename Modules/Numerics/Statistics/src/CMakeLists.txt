set(ITK-Statistics_SRC
itkDenseFrequencyContainer2.cxx
itkSparseFrequencyContainer2.cxx
itkChiSquareDistribution.cxx
itkGaussianDistribution.cxx
itkTDistribution.cxx
itkNormalVariateGenerator.cxx
itkDecisionRule.cxx
itkMaximumDecisionRule.cxx
itkMaximumRatioDecisionRule.cxx
itkMinimumDecisionRule.cxx
)

add_library(ITK-Statistics ${ITK-Statistics_SRC})
target_link_libraries(ITK-Statistics  ${ITK-Common_LIBRARIES} ${ITK-Netlib_LIBRARIES})
itk_module_target(ITK-Statistics)
