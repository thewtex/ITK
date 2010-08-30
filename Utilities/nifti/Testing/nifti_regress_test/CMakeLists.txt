# Do all regression tests

set(NT ${NIFTI_BINARY_DIR}/utils/nifti_tool )
set(DATA  ${NIFTI_TESTS_BINARY_DIR}/nifti_regress_test/nifti_regress_data)
set(TESTS ${NIFTI_TESTS_SOURCE_DIR}/nifti_regress_test/cmake_testscripts)

add_test(fetch_data
sh ${TESTS}/fetch_data_test.sh ${NIFTI_TESTS_BINARY_DIR}/nifti_regress_test
)

add_test(nifti_ver ${NT} -nifti_ver)
add_test(nifti_tool_ver ${NT} -ver)
add_test(nifti_help ${NT} -help)
add_test(nifti_nifti_hist ${NT} -nifti_hist)
add_test(nifti_hist ${NT} -hist)

add_test(nifti_disp_hdr ${NT} -disp_hdr -infiles ${DATA}/anat0.nii )

add_test(nifti_disp_nim ${NT} -disp_nim -infiles ${DATA}/anat0.nii )

add_test(nifti_disp_ext ${NT} -disp_ext -infiles ${DATA}/anat0.nii )

add_test(nifti_modhdr_exts
sh ${TESTS}/mod_header_test.sh ${NT} ${DATA} )

add_test(nifti_bricks_test
sh ${TESTS}/bricks_test.sh ${NT} ${DATA} )

add_test(nifti_dts_test
sh ${TESTS}/dts_test.sh ${NT} ${DATA} )

add_test(nifti_dci_test
sh ${TESTS}/dci_test.sh ${NT} ${DATA} )

add_test(nifti_comment_test
sh ${TESTS}/comment_test.sh ${NT} ${DATA} )

add_test(nifti_header_check ${NT} -check_hdr -infiles ${DATA}/anat0.nii )

add_test(nifti_nim_check ${NT} -check_nim -infiles ${DATA}/anat0.nii )

add_test(nifti_dsets_test
sh ${TESTS}/dsets_test.sh ${NT} ${DATA} )

add_test(nifti_newfiles_test
sh ${TESTS}/newfiles_test.sh ${NT} ${DATA} )
