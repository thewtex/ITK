set(BASELINE ${ITK_DATA_ROOT}/Baseline/Iterators)
set(TEMP ${ITK_BINARY_DIR}/Testing/Temporary)

itk_add_test(NAME ImageLinearIteratorWithIndexTest COMMAND ${ITK_TEST_DRIVER}
  --compare DATA{${BASELINE}/ImageLinearIteratorWithIndexTest.png}
            ${TEMP}/ImageLinearIteratorWithIndexTest.png
  $<TARGET_FILE:ImageLinearIteratorWithIndex>
        ${ITK_SOFTWARE_GUIDE_DATA_ROOT}/VisibleWomanEyeSlice.png
        ${TEMP}/ImageLinearIteratorWithIndexTest.png
)

itk_add_test(NAME ImageRegionIteratorWithIndexTest COMMAND ${ITK_TEST_DRIVER}
  --compare DATA{${BASELINE}/ImageRegionIteratorWithIndexTest.png}
            ${TEMP}/ImageRegionIteratorWithIndexTest.png
  $<TARGET_FILE:ImageRegionIteratorWithIndex>
        ${ITK_SOFTWARE_GUIDE_DATA_ROOT}/VisibleWomanEyeSlice.png
        ${TEMP}/ImageRegionIteratorWithIndexTest.png
)

itk_add_test(NAME ImageRegionIteratorTest COMMAND ${ITK_TEST_DRIVER}
  --compare DATA{${BASELINE}/ImageRegionIteratorTest.png}
            ${TEMP}/ImageRegionIteratorTest.png
  $<TARGET_FILE:ImageRegionIterator>
        ${ITK_SOFTWARE_GUIDE_DATA_ROOT}/FatMRISlice.png
        ${TEMP}/ImageRegionIteratorTest.png
        20 70 210 140
)

itk_add_test(NAME ImageSliceIteratorWithIndexTest COMMAND ${ITK_TEST_DRIVER}
  --compare DATA{${BASELINE}/ImageSliceIteratorWithIndexTest.mha}
            ${TEMP}/ImageSliceIteratorWithIndexTest.mha
 $<TARGET_FILE:ImageSliceIteratorWithIndex>
        ${ITK_SOFTWARE_GUIDE_DATA_ROOT}/BrainProtonDensity3Slices.mha
        ${TEMP}/ImageSliceIteratorWithIndexTest.mha
        2
)
