itk_module_test()
set(ITK_BioCellTests
itkBioCellTest.cxx
itkBioGenomeTest.cxx
itkBioCellularAggregateTest.cxx
itkBioGeneNetworkTest.cxx
itkBioCellHeaderTest.cxx
itkBioGeneTest.cxx
)

CreateTestDriver(ITK_BioCell  "${ITK_BioCell-Test_LIBRARIES}" "${ITK_BioCellTests}")

itk_add_test(NAME itkBioCellHeaderTest
      COMMAND ITK_BioCellTestDriver itkBioCellHeaderTest)
itk_add_test(NAME itkBioCellTest
      COMMAND ITK_BioCellTestDriver itkBioCellTest)
itk_add_test(NAME itkBioGenomeTest
      COMMAND ITK_BioCellTestDriver itkBioGenomeTest)
itk_add_test(NAME itkBioCellularAggregateTest
      COMMAND ITK_BioCellTestDriver itkBioCellularAggregateTest)
itk_add_test(NAME itkBioGeneNetworkTest
      COMMAND ITK_BioCellTestDriver itkBioGeneNetworkTest)
itk_add_test(NAME itkBioGeneTest
      COMMAND ITK_BioCellTestDriver itkBioGeneTest)
