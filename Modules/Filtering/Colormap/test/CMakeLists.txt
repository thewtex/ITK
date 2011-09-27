itk_module_test()
set(ITKColorMapTests
itkScalarToRGBColormapImageFilterTest.cxx
)

CreateTestDriver_SupportBuildInIOFactories(ITKColorMap  "${ITKColorMap-Test_LIBRARIES}" "${ITKColorMapTests}")

itk_add_test(NAME RGBColormapTest_red
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_red.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_red.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_red.png red)
itk_add_test(NAME RGBColormapTest_green
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_green.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_green.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_green.png green)
itk_add_test(NAME RGBColormapTest_blue
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_blue.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_blue.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_blue.png blue)
itk_add_test(NAME RGBColormapTest_hot
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_hot.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_hot.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_hot.png hot)
itk_add_test(NAME RGBColormapTest_cool
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_cool.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_cool.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_cool.png cool)
itk_add_test(NAME RGBColormapTest_spring
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_spring.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_spring.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_spring.png spring)
itk_add_test(NAME RGBColormapTest_summer
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_summer.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_summer.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_summer.png summer)
itk_add_test(NAME RGBColormapTest_autumn
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_autumn.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_autumn.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_autumn.png autumn)
itk_add_test(NAME RGBColormapTest_winter
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_winter.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_winter.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_winter.png winter)
itk_add_test(NAME RGBColormapTest_copper
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_copper.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_copper.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_copper.png copper)
itk_add_test(NAME RGBColormapTest_hsv
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_hsv.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_hsv.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_hsv.png hsv)
itk_add_test(NAME RGBColormapTest_jet
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_jet.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_jet.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_jet.png jet)
itk_add_test(NAME RGBColormapTest_overunder
      COMMAND ITKColorMapTestDriver
    --compare ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_overunder.png
              ${ITK_DATA_ROOT}/Baseline/Review/RGBColormapTest_overunder.png
    itkScalarToRGBColormapImageFilterTest ${ITK_DATA_ROOT}/Input/Colormap_Grey.png ${ITK_TEST_OUTPUT_DIR}/RGBColormapTest_overunder.png overunder)
