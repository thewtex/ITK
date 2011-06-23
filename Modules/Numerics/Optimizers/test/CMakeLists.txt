itk_module_test()
set(ITK_OptimizersTests
itkFRPROptimizerTest.cxx
itkPowellOptimizerTest.cxx
itkGradientDescentOptimizerTest.cxx
itkVersorTransformOptimizerTest.cxx
itkSPSAOptimizerTest.cxx
itkOptimizersHeaderTest.cxx
itkOptimizersHierarchyTest.cxx
itkExhaustiveOptimizerTest.cxx
itkLBFGSBOptimizerTest.cxx
itkConjugateGradientOptimizerTest.cxx
itkLBFGSOptimizerTest.cxx
itkCumulativeGaussianOptimizerTest.cxx
itkRegularStepGradientDescentOptimizerTest.cxx
itkLevenbergMarquardtOptimizerTest.cxx
itkVersorRigid3DTransformOptimizerTest.cxx
itkAmoebaOptimizerTest.cxx
)

CreateTestDriver(ITK_Optimizers  "${ITK_Optimizers-Test_LIBRARIES}" "${ITK_OptimizersTests}")

itk_add_test(NAME itkOptimizersHeaderTest
      COMMAND ITK_OptimizersTestDriver itkOptimizersHeaderTest)
itk_add_test(NAME itkOptimizersHierarchyTest
      COMMAND ITK_OptimizersTestDriver itkOptimizersHierarchyTest)
itk_add_test(NAME itkFRPROptimizerTest
      COMMAND ITK_OptimizersTestDriver itkFRPROptimizerTest)
itk_add_test(NAME itkPowellOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkPowellOptimizerTest)
itk_add_test(NAME itkGradientDescentOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkGradientDescentOptimizerTest)
itk_add_test(NAME itkVersorTransformOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkVersorTransformOptimizerTest)
itk_add_test(NAME itkSPSAOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkSPSAOptimizerTest)
itk_add_test(NAME itkExhaustiveOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkExhaustiveOptimizerTest)
itk_add_test(NAME itkLBFGSBOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkLBFGSBOptimizerTest)
itk_add_test(NAME itkConjugateGradientOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkConjugateGradientOptimizerTest)
itk_add_test(NAME itkLBFGSOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkLBFGSOptimizerTest)
itk_add_test(NAME itkCumulativeGaussianOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkCumulativeGaussianOptimizerTest)
itk_add_test(NAME itkRegularStepGradientDescentOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkRegularStepGradientDescentOptimizerTest)
itk_add_test(NAME itkLevenbergMarquardtOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkLevenbergMarquardtOptimizerTest)
itk_add_test(NAME itkVersorRigid3DTransformOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkVersorRigid3DTransformOptimizerTest)
itk_add_test(NAME itkAmoebaOptimizerTest
      COMMAND ITK_OptimizersTestDriver itkAmoebaOptimizerTest)
