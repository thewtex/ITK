itk_module_test()
set(ITK-AffineImageTests
itkAffineImageTest.cxx
)
CreateTestDriver(ITK-AffineImage "ITKOptimizers" "${ITK-AffineImageTests}")
itk_add_test( NAME itkAffineImageTest COMMAND ITK-AffineImageTestDriver itkAffineImageTest )
