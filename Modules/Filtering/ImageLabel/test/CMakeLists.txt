itk_module_test()
set(ITK_ImageLabelTests
itkChangeLabelImageFilterTest.cxx
itkLabelContourImageFilterTest.cxx
itkBinaryContourImageFilterTest.cxx
itkImageLabelHeaderTest.cxx
)

CreateTestDriver(ITK_ImageLabel  "${ITK_ImageLabel-Test_LIBRARIES}" "${ITK_ImageLabelTests}")

itk_add_test(NAME itkImageLabelHeaderTest
      COMMAND ITK_ImageLabelTestDriver itkImageLabelHeaderTest)
itk_add_test(NAME itkChangeLabelImageFilterTest
      COMMAND ITK_ImageLabelTestDriver itkChangeLabelImageFilterTest)
itk_add_test(NAME itkLabelContourImageFilterTest0
      COMMAND ITK_ImageLabelTestDriver
      --compare ${ITK_DATA_ROOT}/Baseline/Review/itkLabelContourImageFilterTest0.png
      ${ITK_TEST_OUTPUT_DIR}/itkLabelContourImageFilterTest0.png
      itkLabelContourImageFilterTest
      ${ITK_DATA_ROOT}/Input/2th_cthead1.png
      ${ITK_TEST_OUTPUT_DIR}/itkLabelContourImageFilterTest0.png
      0 0)
itk_add_test(NAME itkLabelContourImageFilterTest1
      COMMAND ITK_ImageLabelTestDriver
      --compare ${ITK_DATA_ROOT}/Baseline/Review/itkLabelContourImageFilterTest1.png
      ${ITK_TEST_OUTPUT_DIR}/itkLabelContourImageFilterTest1.png
      itkLabelContourImageFilterTest
      ${ITK_DATA_ROOT}/Input/2th_cthead1.png
      ${ITK_TEST_OUTPUT_DIR}/itkLabelContourImageFilterTest1.png
      1 0)
itk_add_test(NAME itkBinaryContourImageFilterTest0
      COMMAND ITK_ImageLabelTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Review/itkBinaryContourImageFilterTest0.png
              ${ITK_TEST_OUTPUT_DIR}/itkBinaryContourImageFilterTest0.png
    itkBinaryContourImageFilterTest ${ITK_DATA_ROOT}/Input/2th_cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkBinaryContourImageFilterTest0.png 0 200 0)
itk_add_test(NAME itkBinaryContourImageFilterTest1
      COMMAND ITK_ImageLabelTestDriver
    --compare ${ITK_DATA_ROOT}/Baseline/Review/itkBinaryContourImageFilterTest1.png
              ${ITK_TEST_OUTPUT_DIR}/itkBinaryContourImageFilterTest1.png
    itkBinaryContourImageFilterTest ${ITK_DATA_ROOT}/Input/2th_cthead1.png ${ITK_TEST_OUTPUT_DIR}/itkBinaryContourImageFilterTest1.png 1 200 0)
