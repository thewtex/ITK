set(ITK_Polynomials_SRC
itkMultivariateLegendrePolynomial.cxx
)

add_library(ITK_Polynomials ${ITK_Polynomials_SRC})
target_link_libraries(ITK_Polynomials  ${ITK_Common_LIBRARIES})
itk_module_target(ITK_Polynomials)
