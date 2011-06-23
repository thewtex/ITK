itk_module_test()
set(ITK_LabelVotingTests
itkLabelVotingHeaderTest.cxx
itkVotingBinaryImageFilterTest.cxx
itkLabelVotingImageFilterTest.cxx
itkVotingBinaryIterativeHoleFillingImageFilterTest.cxx
itkBinaryMedianImageFilterTest.cxx
itkVotingBinaryHoleFillingImageFilterTest.cxx
)

CreateTestDriver(ITK_LabelVoting  "${ITK_LabelVoting-Test_LIBRARIES}" "${ITK_LabelVotingTests}")

itk_add_test(NAME itkLabelVotingHeaderTest
      COMMAND ITK_LabelVotingTestDriver itkLabelVotingHeaderTest)
itk_add_test(NAME itkVotingBinaryImageFilterTest
      COMMAND ITK_LabelVotingTestDriver itkVotingBinaryImageFilterTest)
itk_add_test(NAME itkLabelVotingImageFilterTest
      COMMAND ITK_LabelVotingTestDriver itkLabelVotingImageFilterTest)
itk_add_test(NAME itkVotingBinaryIterativeHoleFillingImageFilterTest
      COMMAND ITK_LabelVotingTestDriver itkVotingBinaryIterativeHoleFillingImageFilterTest)
itk_add_test(NAME itkBinaryMedianImageFilterTest
      COMMAND ITK_LabelVotingTestDriver itkBinaryMedianImageFilterTest)
itk_add_test(NAME itkVotingBinaryHoleFillingImageFilterTest
      COMMAND ITK_LabelVotingTestDriver itkVotingBinaryHoleFillingImageFilterTest)
