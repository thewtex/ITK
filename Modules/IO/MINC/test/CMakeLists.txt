itk_module_test()

FIND_PACKAGE( MINC2 REQUIRED )

INCLUDE_DIRECTORIES(${MINC2_INCLUDE_DIRS})
LINK_DIRECTORIES(${MINC2_LIBRARY_DIRS})

set(ITKIOMINCTests
   itkMINCImageIOTest.cxx
)

set(ITKIOTransformMincTests
   itkIOTransformMincTest.cxx
)

CreateTestDriver(ITKIOMINC  "${ITKIOMINC-Test_LIBRARIES}" "${ITKIOMINCTests}")

itk_add_test(NAME itkMINCImageIOTest
  COMMAND ITKIOMINCTestDriver itkMINCImageIOTest ${ITK_TEST_OUTPUT_DIR} )

CreateTestDriver(ITKIOTransformMINC "${ITKIOMINC-Test_LIBRARIES}" "${ITKIOTransformMincTests}")
TARGET_LINK_LIBRARIES(ITKIOTransformMINCTestDriver ITKIOTransformBase )

itk_add_test(NAME itkIOTransformMINCTest
      COMMAND ITKIOTransformMINCTestDriver itkIOTransformMincTest)
