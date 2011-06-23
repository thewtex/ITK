set(ITK_Path_SRC
itkChainCodePath2D.cxx
itkOrthogonallyCorrected2DParametricPath.cxx
itkParametricPath.cxx
)

add_library(ITK_Path ${ITK_Path_SRC})
target_link_libraries(ITK_Path  ${ITK_ImageFilterBase_LIBRARIES} ${ITK_ImageFunction_LIBRARIES})
itk_module_target(ITK_Path)
