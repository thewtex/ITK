itk_module_test()
set(ITK_BloxTests
itkBloxCoreAtomTest.cxx
itkBloxBoundaryPointImageTest.cxx
itkBinaryMedialNodeMetricTest.cxx
itkBloxHeaderTest.cxx
itkBloxBoundaryProfileImageToBloxCoreAtomImageFilterTest.cxx
itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilterTest.cxx
itkMedialNodeCorrespondencesTest.cxx
)

CreateTestDriver(ITK_Blox  "${ITK_Blox-Test_LIBRARIES}" "${ITK_BloxTests}")

itk_add_test(NAME itkBloxHeaderTest
      COMMAND ITK_BloxTestDriver itkBloxHeaderTest)
itk_add_test(NAME itkBloxCoreAtomTest
      COMMAND ITK_BloxTestDriver itkBloxCoreAtomTest)
itk_add_test(NAME itkBloxBoundaryPointImageTest
      COMMAND ITK_BloxTestDriver itkBloxBoundaryPointImageTest)
itk_add_test(NAME itkBinaryMedialNodeMetricTest
      COMMAND ITK_BloxTestDriver itkBinaryMedialNodeMetricTest)
itk_add_test(NAME itkBloxBoundaryProfileImageToBloxCoreAtomImageFilterTest
      COMMAND ITK_BloxTestDriver itkBloxBoundaryProfileImageToBloxCoreAtomImageFilterTest)
itk_add_test(NAME itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilterTest
      COMMAND ITK_BloxTestDriver itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilterTest)
itk_add_test(NAME itkMedialNodeCorrespondencesTest
      COMMAND ITK_BloxTestDriver itkMedialNodeCorrespondencesTest)
