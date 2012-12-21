itk_module_test()

IF(NOT ITK_USE_SYSTEM_HDF5)
  include_directories(
    "${ITKHDF5_SOURCE_DIR}/src/itkhdf5"
    "${ITKHDF5_BINARY_DIR}/src/itkhdf5"
    )
ENDIF()

set(ITKIOMINCTests
   itkMINCImageIOTest.cxx
   itkMINCImageIOTest2.cxx
   itkMINCImageIOTest3.cxx
  )

# set(ITKIOTransformMincTests
#    itkIOTransformMincTest.cxx
# )

CreateTestDriver(ITKIOMINC "${ITKIOMINC-Test_LIBRARIES}" "${ITKIOMINCTests}")

itk_add_test(NAME itkMINCImageIOTest
  COMMAND ITKIOMINCTestDriver itkMINCImageIOTest ${ITK_TEST_OUTPUT_DIR} )

itk_add_test(NAME itkMINCImageIOTest2
  COMMAND ITKIOMINCTestDriver itkMINCImageIOTest2
      DATA{Input/pd_z+_byte_cor.mnc} ${ITK_TEST_OUTPUT_DIR}/pd_z+_byte_cor.mnc)

itk_add_test(NAME itkMINCImageIOTest3
  COMMAND ITKIOMINCTestDriver
      --compare DATA{Input/pd_z+_byte_cor.mnc}
                ${ITK_TEST_OUTPUT_DIR}/pd_z+_byte_cor_2.mnc
  itkMINCImageIOTest3
      DATA{Input/pd_z+_byte_cor.mnc} ${ITK_TEST_OUTPUT_DIR}/pd_z+_byte_cor_2.mnc)

itk_add_test(NAME itkMINCImageIOTest4
  COMMAND ITKIOMINCTestDriver
--compare DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd}
                ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mnc
  itkMINCImageIOTest3
      DATA{${ITK_DATA_ROOT}/Input/HeadMRVolume.mhd} ${ITK_TEST_OUTPUT_DIR}/HeadMRVolume.mnc)

set(MINC_TEST_FILES
  pd_z-_byte_cor.mnc
  pd_z+_byte_cor.mnc
  pd_z-_byte_sag.mnc
  pd_z+_byte_sag.mnc
  pd_z-_byte_trans.mnc
  pd_z+_byte_trans.mnc
  pd_z-_byte_xyz.mnc
  pd_z+_byte_xyz.mnc
  pd_z-_byte_yzx.mnc
  pd_z+_byte_yzx.mnc
  pd_z-_byte_zxy.mnc
  pd_z+_byte_zxy.mnc
  pd_z-_double_cor.mnc
  pd_z+_double_cor.mnc
  pd_z-_double_sag.mnc
  pd_z+_double_sag.mnc
  pd_z-_double_trans.mnc
  pd_z+_double_trans.mnc
  pd_z-_double_xyz.mnc
  pd_z+_double_xyz.mnc
  pd_z-_double_yzx.mnc
  pd_z+_double_yzx.mnc
  pd_z-_double_zxy.mnc
  pd_z+_double_zxy.mnc
  pd_z-_float_cor.mnc
  pd_z+_float_cor.mnc
  pd_z-_float_sag.mnc
  pd_z+_float_sag.mnc
  pd_z-_float_trans.mnc
  pd_z+_float_trans.mnc
  pd_z-_float_xyz.mnc
  pd_z+_float_xyz.mnc
  pd_z-_float_yzx.mnc
  pd_z+_float_yzx.mnc
  pd_z-_float_zxy.mnc
  pd_z+_float_zxy.mnc
  pd_z-_long_cor.mnc
  pd_z+_long_cor.mnc
  pd_z-_long_sag.mnc
  pd_z+_long_sag.mnc
  pd_z-_long_trans.mnc
  pd_z+_long_trans.mnc
  pd_z-_long_xyz.mnc
  pd_z+_long_xyz.mnc
  pd_z-_long_yzx.mnc
  pd_z+_long_yzx.mnc
  pd_z-_long_zxy.mnc
  pd_z+_long_zxy.mnc
  pd_z-_short_cor.mnc
  pd_z+_short_cor.mnc
  pd_z-_short_sag.mnc
  pd_z+_short_sag.mnc
  pd_z-_short_trans.mnc
  pd_z+_short_trans.mnc
  pd_z-_short_xyz.mnc
  pd_z+_short_xyz.mnc
  pd_z-_short_yzx.mnc
  pd_z+_short_yzx.mnc
  pd_z-_short_zxy.mnc
  pd_z+_short_zxy.mnc
  pd_z-_ubyte_cor.mnc
  pd_z+_ubyte_cor.mnc
  pd_z-_ubyte_sag.mnc
  pd_z+_ubyte_sag.mnc
  pd_z-_ubyte_trans.mnc
  pd_z+_ubyte_trans.mnc
  pd_z-_ubyte_xyz.mnc
  pd_z+_ubyte_xyz.mnc
  pd_z-_ubyte_yzx.mnc
  pd_z+_ubyte_yzx.mnc
  pd_z-_ubyte_zxy.mnc
  pd_z+_ubyte_zxy.mnc
  pd_z-_ulong_cor.mnc
  pd_z+_ulong_cor.mnc
  pd_z-_ulong_sag.mnc
  pd_z+_ulong_sag.mnc
  pd_z-_ulong_trans.mnc
  pd_z+_ulong_trans.mnc
  pd_z-_ulong_xyz.mnc
  pd_z+_ulong_xyz.mnc
  pd_z-_ulong_yzx.mnc
  pd_z+_ulong_yzx.mnc
  pd_z-_ulong_zxy.mnc
  pd_z+_ulong_zxy.mnc
  pd_z-_ushort_cor.mnc
  pd_z+_ushort_cor.mnc
  pd_z-_ushort_sag.mnc
  pd_z+_ushort_sag.mnc
  pd_z-_ushort_trans.mnc
  pd_z+_ushort_trans.mnc
  pd_z-_ushort_xyz.mnc
  pd_z+_ushort_xyz.mnc
  pd_z-_ushort_yzx.mnc
  pd_z+_ushort_yzx.mnc
  pd_z-_ushort_zxy.mnc
  pd_z+_ushort_zxy.mnc
  t1_z-_byte_cor.mnc
  t1_z+_byte_cor.mnc
  t1_z-_byte_sag.mnc
  t1_z+_byte_sag.mnc
  t1_z-_byte_trans.mnc
  t1_z+_byte_trans.mnc
  t1_z-_byte_xyz.mnc
  t1_z+_byte_xyz.mnc
  t1_z-_byte_yzx.mnc
  t1_z+_byte_yzx.mnc
  t1_z-_byte_zxy.mnc
  t1_z+_byte_zxy.mnc
  t1_z-_double_cor.mnc
  t1_z+_double_cor.mnc
  t1_z-_double_sag.mnc
  t1_z+_double_sag.mnc
  t1_z-_double_trans.mnc
  t1_z+_double_trans.mnc
  t1_z-_double_xyz.mnc
  t1_z+_double_xyz.mnc
  t1_z-_double_yzx.mnc
  t1_z+_double_yzx.mnc
  t1_z-_double_zxy.mnc
  t1_z+_double_zxy.mnc
  t1_z-_float_cor.mnc
  t1_z+_float_cor.mnc
  t1_z-_float_sag.mnc
  t1_z+_float_sag.mnc
  t1_z-_float_trans.mnc
  t1_z+_float_trans.mnc
  t1_z-_float_xyz.mnc
  t1_z+_float_xyz.mnc
  t1_z-_float_yzx.mnc
  t1_z+_float_yzx.mnc
  t1_z-_float_zxy.mnc
  t1_z+_float_zxy.mnc
  t1_z-_long_cor.mnc
  t1_z+_long_cor.mnc
  t1_z-_long_sag.mnc
  t1_z+_long_sag.mnc
  t1_z-_long_trans.mnc
  t1_z+_long_trans.mnc
  t1_z-_long_xyz.mnc
  t1_z+_long_xyz.mnc
  t1_z-_long_yzx.mnc
  t1_z+_long_yzx.mnc
  t1_z-_long_zxy.mnc
  t1_z+_long_zxy.mnc
  t1_z-_short_cor.mnc
  t1_z+_short_cor.mnc
  t1_z-_short_sag.mnc
  t1_z+_short_sag.mnc
  t1_z-_short_trans.mnc
  t1_z+_short_trans.mnc
  t1_z-_short_xyz.mnc
  t1_z+_short_xyz.mnc
  t1_z-_short_yzx.mnc
  t1_z+_short_yzx.mnc
  t1_z-_short_zxy.mnc
  t1_z+_short_zxy.mnc
  t1_z-_ubyte_cor.mnc
  t1_z+_ubyte_cor.mnc
  t1_z-_ubyte_sag.mnc
  t1_z+_ubyte_sag.mnc
  t1_z-_ubyte_trans.mnc
  t1_z+_ubyte_trans.mnc
  t1_z-_ubyte_xyz.mnc
  t1_z+_ubyte_xyz.mnc
  t1_z-_ubyte_yzx.mnc
  t1_z+_ubyte_yzx.mnc
  t1_z-_ubyte_zxy.mnc
  t1_z+_ubyte_zxy.mnc
  t1_z-_ulong_cor.mnc
  t1_z+_ulong_cor.mnc
  t1_z-_ulong_sag.mnc
  t1_z+_ulong_sag.mnc
  t1_z-_ulong_trans.mnc
  t1_z+_ulong_trans.mnc
  t1_z-_ulong_xyz.mnc
  t1_z+_ulong_xyz.mnc
  t1_z-_ulong_yzx.mnc
  t1_z+_ulong_yzx.mnc
  t1_z-_ulong_zxy.mnc
  t1_z+_ulong_zxy.mnc
  t1_z-_ushort_cor.mnc
  t1_z+_ushort_cor.mnc
  t1_z-_ushort_sag.mnc
  t1_z+_ushort_sag.mnc
  t1_z-_ushort_trans.mnc
  t1_z+_ushort_trans.mnc
  t1_z-_ushort_xyz.mnc
  t1_z+_ushort_xyz.mnc
  t1_z-_ushort_yzx.mnc
  t1_z+_ushort_yzx.mnc
  t1_z-_ushort_zxy.mnc
  t1_z+_ushort_zxy.mnc
  t2_z-_byte_cor.mnc
  t2_z+_byte_cor.mnc
  t2_z-_byte_sag.mnc
  t2_z+_byte_sag.mnc
  t2_z-_byte_trans.mnc
  t2_z+_byte_trans.mnc
  t2_z-_byte_xyz.mnc
  t2_z+_byte_xyz.mnc
  t2_z-_byte_yzx.mnc
  t2_z+_byte_yzx.mnc
  t2_z-_byte_zxy.mnc
  t2_z+_byte_zxy.mnc
  t2_z-_double_cor.mnc
  t2_z+_double_cor.mnc
  t2_z-_double_sag.mnc
  t2_z+_double_sag.mnc
  t2_z-_double_trans.mnc
  t2_z+_double_trans.mnc
  t2_z-_double_xyz.mnc
  t2_z+_double_xyz.mnc
  t2_z-_double_yzx.mnc
  t2_z+_double_yzx.mnc
  t2_z-_double_zxy.mnc
  t2_z+_double_zxy.mnc
  t2_z-_float_cor.mnc
  t2_z+_float_cor.mnc
  t2_z-_float_sag.mnc
  t2_z+_float_sag.mnc
  t2_z-_float_trans.mnc
  t2_z+_float_trans.mnc
  t2_z-_float_xyz.mnc
  t2_z+_float_xyz.mnc
  t2_z-_float_yzx.mnc
  t2_z+_float_yzx.mnc
  t2_z-_float_zxy.mnc
  t2_z+_float_zxy.mnc
  t2_z-_long_cor.mnc
  t2_z+_long_cor.mnc
  t2_z-_long_sag.mnc
  t2_z+_long_sag.mnc
  t2_z-_long_trans.mnc
  t2_z+_long_trans.mnc
  t2_z-_long_xyz.mnc
  t2_z+_long_xyz.mnc
  t2_z-_long_yzx.mnc
  t2_z+_long_yzx.mnc
  t2_z-_long_zxy.mnc
  t2_z+_long_zxy.mnc
  t2_z-_short_cor.mnc
  t2_z+_short_cor.mnc
  t2_z-_short_sag.mnc
  t2_z+_short_sag.mnc
  t2_z-_short_trans.mnc
  t2_z+_short_trans.mnc
  t2_z-_short_xyz.mnc
  t2_z+_short_xyz.mnc
  t2_z-_short_yzx.mnc
  t2_z+_short_yzx.mnc
  t2_z-_short_zxy.mnc
  t2_z+_short_zxy.mnc
  t2_z-_ubyte_cor.mnc
  t2_z+_ubyte_cor.mnc
  t2_z-_ubyte_sag.mnc
  t2_z+_ubyte_sag.mnc
  t2_z-_ubyte_trans.mnc
  t2_z+_ubyte_trans.mnc
  t2_z-_ubyte_xyz.mnc
  t2_z+_ubyte_xyz.mnc
  t2_z-_ubyte_yzx.mnc
  t2_z+_ubyte_yzx.mnc
  t2_z-_ubyte_zxy.mnc
  t2_z+_ubyte_zxy.mnc
  t2_z-_ulong_cor.mnc
  t2_z+_ulong_cor.mnc
  t2_z-_ulong_sag.mnc
  t2_z+_ulong_sag.mnc
  t2_z-_ulong_trans.mnc
  t2_z+_ulong_trans.mnc
  t2_z-_ulong_xyz.mnc
  t2_z+_ulong_xyz.mnc
  t2_z-_ulong_yzx.mnc
  t2_z+_ulong_yzx.mnc
  t2_z-_ulong_zxy.mnc
  t2_z+_ulong_zxy.mnc
  t2_z-_ushort_cor.mnc
  t2_z+_ushort_cor.mnc
  t2_z-_ushort_sag.mnc
  t2_z+_ushort_sag.mnc
  t2_z-_ushort_trans.mnc
  t2_z+_ushort_trans.mnc
  t2_z-_ushort_xyz.mnc
  t2_z+_ushort_xyz.mnc
  t2_z-_ushort_yzx.mnc
  t2_z+_ushort_yzx.mnc
  t2_z-_ushort_zxy.mnc
  t2_z+_ushort_zxy.mnc
  )

foreach(MINC_TEST_FILE IN LISTS MINC_TEST_FILES)
  itk_add_test(NAME itkMINCImageIOTest-${MINC_TEST_FILE}
    COMMAND ITKIOMINCTestDriver
      --compare DATA{Input/${MINC_TEST_FILE}}
                  ${ITK_TEST_OUTPUT_DIR}/${MINC_TEST_FILE}
    itkMINCImageIOTest3
      DATA{Input/${MINC_TEST_FILE}}
      ${ITK_TEST_OUTPUT_DIR}/${MINC_TEST_FILE}
    )
endforeach(MINC_TEST_FILE)


# CreateTestDriver(ITKIOTransformMINC "${ITKIOMINC-Test_LIBRARIES}" "${ITKIOTransformMincTests}")
# TARGET_LINK_LIBRARIES(ITKIOTransformMINCTestDriver ITKIOTransformBase )
#
# itk_add_test(NAME itkIOTransformMINCTest
#       COMMAND ITKIOTransformMINCTestDriver itkIOTransformMincTest)

