itk_module_test()
set(ITK_PDEDeformableRegistrationTests
itkMultiResolutionPDEDeformableRegistrationTest.cxx
itkPDEDeformableRegistrationHeaderTest.cxx
itkDemonsRegistrationFilterTest.cxx
itkLevelSetMotionRegistrationFilterTest.cxx
itkSymmetricForcesDemonsRegistrationFilterTest.cxx
)
 # Define some convenient locations
set(BASELINE ${ITK_DATA_ROOT}/Baseline/Algorithms)
set(INPUTDATA ${ITK_DATA_ROOT}/Input)
set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITK_PDEDeformableRegistration  "${ITK_PDEDeformableRegistration-Test_LIBRARIES}" "${ITK_PDEDeformableRegistrationTests}")

itk_add_test(NAME itkPDEDeformableRegistrationHeaderTest
      COMMAND ITK_PDEDeformableRegistrationTestDriver itkPDEDeformableRegistrationHeaderTest)
itk_add_test(NAME itkDemonsRegistrationFilterTest
      COMMAND ITK_PDEDeformableRegistrationTestDriver itkDemonsRegistrationFilterTest)
itk_add_test(NAME itkLevelSetMotionRegistrationFilterTest
      COMMAND ITK_PDEDeformableRegistrationTestDriver itkLevelSetMotionRegistrationFilterTest
              ${ITK_TEST_OUTPUT_DIR}/itkLevelSetMotionRegistrationFilterTestFixedImage.mha ${ITK_TEST_OUTPUT_DIR}/itkLevelSetMotionRegistrationFilterTestMovingImage.mha ${ITK_TEST_OUTPUT_DIR}/itkLevelSetMotionRegistrationFilterTestResampledImage.mha)
itk_add_test(NAME itkSymmetricForcesDemonsRegistrationFilterTest
      COMMAND ITK_PDEDeformableRegistrationTestDriver itkSymmetricForcesDemonsRegistrationFilterTest)
itk_add_test(NAME itkMultiResolutionPDEDeformableRegistrationTestD ${TestDriver}
      COMMAND ITK_PDEDeformableRegistrationTestDriver
            --compare ${BASELINE}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png
            ${TEMP}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png
  --compareNumberOfPixelsTolerance 20
  itkMultiResolutionPDEDeformableRegistrationTest
            ${TEMP}/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png)
