itk_module_test()
set(ITK_ImageFilterBaseTests
itkImageFilterBaseHeaderTest.cxx
itkNeighborhoodOperatorImageFilterTest.cxx
itkImageToImageFilterTest.cxx
itkVectorNeighborhoodOperatorImageFilterTest.cxx
itkGaussianImageSourceTest.cxx
itkMaskNeighborhoodOperatorImageFilterTest.cxx
itkCastImageFilterTest.cxx
itkClampImageFilterTest.cxx
)

# Disable optimization on the tests below to avoid possible
# optimization bugs in some compilers. This option will appear on the
# command line after any other optimization flag (including those from
# CMAKE_CXX_FLAGS) which means that on gcc, MSVC, and the Intel
# compiler, it will override any other optimization flag.
if (DEFINED ITK_CXX_DISABLE_OPTIMIZATION_FLAG)
  set_source_files_properties(
    itkCastImageFilterTest.cxx
    itkClampImageFilterTest.cxx
    PROPERTIES COMPILE_FLAGS ${ITK_CXX_DISABLE_OPTIMIZATION_FLAG}
  )
endif()


CreateTestDriver(ITK_ImageFilterBase  "${ITK_ImageFilterBase-Test_LIBRARIES}" "${ITK_ImageFilterBaseTests}")

itk_add_test(NAME itkImageFilterBaseHeaderTest
      COMMAND ITK_ImageFilterBaseTestDriver itkImageFilterBaseHeaderTest)
itk_add_test(NAME itkNeighborhoodOperatorImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver itkNeighborhoodOperatorImageFilterTest)
itk_add_test(NAME itkImageToImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver itkImageToImageFilterTest)
itk_add_test(NAME itkVectorNeighborhoodOperatorImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver itkVectorNeighborhoodOperatorImageFilterTest)
itk_add_test(NAME itkGaussianImageSourceTest
      COMMAND ITK_ImageFilterBaseTestDriver itkGaussianImageSourceTest)
itk_add_test(NAME itkMaskNeighborhoodOperatorImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/BasicFilters/MaskNeighborhoodOperatorImageFilterTest.png
              ${ITK_TEST_OUTPUT_DIR}/MaskNeighborhoodOperatorImageFilterTest.png
    itkMaskNeighborhoodOperatorImageFilterTest ${ITK_DATA_ROOT}/Input/cthead1.png ${ITK_TEST_OUTPUT_DIR}/MaskNeighborhoodOperatorImageFilterTest.png)
itk_add_test(NAME itkCastImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver itkCastImageFilterTest)
itk_add_test(NAME itkClampImageFilterTest
      COMMAND ITK_ImageFilterBaseTestDriver itkClampImageFilterTest)
