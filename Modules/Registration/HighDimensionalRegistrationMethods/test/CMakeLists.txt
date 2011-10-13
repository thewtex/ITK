itk_module_test()
set(ITKHighDimensionalRegistrationMethodsTests
itkSimpleImageRegistrationTest.cxx
)

set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(BASELINE_ROOT ${ITK_DATA_ROOT}/Baseline)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITKHighDimensionalRegistrationMethods  "${ITKHighDimensionalRegistrationMethods-Test_LIBRARIES}" "${ITKHighDimensionalRegistrationMethodsTests}")

itk_add_test(NAME itkSimpleImageRegistrationTest
      COMMAND ITKHighDimensionalRegistrationMethodsTestDriver itkSimpleImageRegistrationTest 2
              DATA{Baseline/r16slice_rigid.nii.gz}
              DATA{Baseline/r64slice.nii.gz}
              ${TEMP}/itkSimpleImageRegistrationTest.nii.gz)
