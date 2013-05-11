itk_module_test()
set(ITKIOMGHTests
    itkMGHImageIOTest.cxx
)

set(MGH_DATA_ROOT ${CMAKE_CURRENT_DIR}/MD5)

# For itkMGHImageIOTest.h.
include_directories( ${ITKIOMGH_SOURCE_DIR}/test )

CreateTestDriver(ITKIOMGH  "${ITKIOMGH-Test_LIBRARIES}" "${ITKIOMGHTests}")

if(0) #TODO: Need to add tests here
itk_add_test(NAME T1.mgz
      COMMAND ITKIOMGHTestDriver itkMGHImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkMGHIOBigEndianCompressed.hdr DATA{${MGH_DATA_ROOT}/T1.mgz})
itk_add_test(NAME T1_longname.mgh.gz
      COMMAND ITKIOMGHTestDriver itkMGHImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkMGHIOBigEndianCompressed.img DATA{${MGH_DATA_ROOT}/T1_longname.mgh.gz})
itk_add_test(NAME T1_uncompresed.mgh
      COMMAND ITKIOMGHTestDriver itkMGHImageIOTest
              ${ITK_TEST_OUTPUT_DIR} itkMGHIOBigEndianCompressed.img.gz DATA{${MGH_DATA_ROOT}/T1_uncompresed.mgh})

itk_add_test(NAME itkMGHIOInternalTests
      COMMAND ITKIOMGHTestDriver itkMGHImageIOTest
              ${ITK_TEST_OUTPUT_DIR})
endif()
