itk_module_test()
set(ITK_DeformationFieldTests
itkDisplacementFieldJacobianDeterminantFilterTest.cxx
itkDeformationFieldHeaderTest.cxx
itkDeformationFieldJacobianDeterminantFilterTest.cxx
itkIterativeInverseDeformationFieldImageFilterTest.cxx
itkDeformationFieldSourceTest.cxx
itkInverseDeformationFieldImageFilterTest.cxx
)

CreateTestDriver(ITK_DeformationField  "${ITK_DeformationField-Test_LIBRARIES}" "${ITK_DeformationFieldTests}")

itk_add_test(NAME itkDeformationFieldHeaderTest
      COMMAND ITK_DeformationFieldTestDriver itkDeformationFieldHeaderTest)
itk_add_test(NAME itkDisplacementFieldJacobianDeterminantFilterTest
      COMMAND ITK_DeformationFieldTestDriver itkDisplacementFieldJacobianDeterminantFilterTest)
itk_add_test(NAME itkDeformationFieldJacobianDeterminantFilterTest
      COMMAND ITK_DeformationFieldTestDriver itkDeformationFieldJacobianDeterminantFilterTest)
itk_add_test(NAME itkIterativeInverseDeformationFieldImageFilterTest
      COMMAND ITK_DeformationFieldTestDriver itkIterativeInverseDeformationFieldImageFilterTest
              ${ITK_TEST_OUTPUT_DIR}/itkIterativeInverseDeformationFieldImageFilterTest.mha)
itk_add_test(NAME itkDeformationFieldSourceTest
      COMMAND ITK_DeformationFieldTestDriver itkDeformationFieldSourceTest
              ${ITK_TEST_OUTPUT_DIR}/itkDeformationFieldSourceTestLandmarks.txt ${ITK_TEST_OUTPUT_DIR}/itkDeformationFieldSourceTest.mha)
itk_add_test(NAME itkInverseDeformationFieldImageFilterTest
      COMMAND ITK_DeformationFieldTestDriver itkInverseDeformationFieldImageFilterTest
              ${ITK_TEST_OUTPUT_DIR}/itkInverseDeformationFieldImageFilterTest.mha)
