set(TEMP ${ITK_BINARY_DIR}/Testing/Temporary)

if(ITK_WRAP_PYTHON)
  INTERSECTION(WRAP_2 2 "${ITK_WRAP_DIMS}")

  if(ITK_WRAP_signed_short AND ITK_WRAP_unsigned_char AND WRAP_2)
    add_python_test(DicomSliceReadPythonTest
      ${CMAKE_CURRENT_SOURCE_DIR}/../DicomSliceRead.py
      DATA{${ITK_DATA_ROOT}/Input/dicom-sc_cs-1.dcm}
      ${TEMP}/DicomSliceReadPythonOutput.png
      --compare ${TEMP}/DicomSliceReadPythonOutput.png
        DATA{${ITK_SOURCE_DIR}/Examples/IO/test/Baseline/DicomSliceReadPythonOutputBaseline.png}
      )
  endif()
endif()

if(ITK_BUILD_SHARED_LIBS)
  itk_add_test( NAME ExampleIOPluginTest
    COMMAND itkTestDriver  --add-before-env ITK_AUTOLOAD_PATH "$<TARGET_FILE_DIR:FileFreeTestLib>"
      "$<TARGET_FILE:IOPlugin>"
      "FileFreeIO::Size=128,256:Spacing=.5,.8:Origin=5,6:Direction=-1,0,0,-1"
      "${ITK_TEST_OUTPUT_DIR}ExampleIOPluginTest.png"
      )
  itk_add_test( NAME ExampleIOPluginTest2
    COMMAND itkTestDriver
      "$<TARGET_FILE:IOPlugin>"
      "FileFreeIO::Size=128,256:Spacing=.5,.8:Origin=5,6:Direction=-1,0,0,-1"
      "${ITK_TEST_OUTPUT_DIR}ExampleIOPluginTest2.png"
      "$<TARGET_FILE_DIR:FileFreeTestLib>"
      )

endif()

# Test ITK ImageIO factory registrations.
# Please refer to CMake/UseITK.cmake for all the image IO registartion handeling.

# Internal default image IO modules (ITK_USE_DEFAULT_MODULES is ON, so all of
# the default IOs should be registered)
# The order needs to be consistent with the order of registration in CMake/UseITK.cmake.
set (LIST_OF_DEFAULT_IMAGE_IO_FORMATS "Nifti Nrrd Gipl HDF5 JPEG GDCM BMP LSM PNG TIFF VTK Stimulate BioRad Meta MRC")
set (List_IO_Factory_Description_First_Word ${LIST_OF_DEFAULT_IMAGE_IO_FORMATS})

# internal non-default image IO modules: MINC
set(module_name ITKIOMINC)
if (${module_name}_ENABLED)
  set (List_IO_Factory_Description_First_Word "${List_IO_Factory_Description_First_Word} MINC")
endif()

# for remote image IO  modules: MGHIO and SCIFIO
set(module_name MGHIO)
if (${module_name}_ENABLED)
  set (List_IO_Factory_Description_First_Word "${List_IO_Factory_Description_First_Word} MGH")
endif()
set(module_name SCIFIO)
if (${module_name}_ENABLED)
  set (List_IO_Factory_Description_First_Word "${List_IO_Factory_Description_First_Word} SCIFIO")
endif()

itk_add_test( NAME ImageIOFactoryRegisterTest
    COMMAND ${ITK_TEST_DRIVER}
    $<TARGET_FILE:IOFactoryRegistration>
    )
# The test output should contain the list of the ImageIOs.
set_tests_properties (ImageIOFactoryRegisterTest
  PROPERTIES PASS_REGULAR_EXPRESSION  ${List_IO_Factory_Description_First_Word})


if(ITK_VISIBLEHUMAN_DATA_ROOT)
  itk_add_test(NAME VisibleHumanStreamReadWriteTest
    COMMAND ${ITK_TEST_DRIVER}
   $<TARGET_FILE:VisibleHumanStreamReadWrite>
    ${ITK_VISIBLEHUMAN_DATA_ROOT}/Male/Fullcolor/fullbody/
    ${TEMP}/VisibleHumanStreamReadWrite.mha )
  itk_add_test(NAME VisibleHumanPasteWriteTest
    COMMAND ${ITK_TEST_DRIVER}
   $<TARGET_FILE:VisibleHumanPasteWrite>
    ${TEMP}/VisibleHumanStreamReadWrite.mha
    ${TEMP}/VisibleHumanPasteWrite.mha)
  # this test must be run sequentially, be cause the output of the first is used by the second
  set_tests_properties(
    VisibleHumanStreamReadWriteTest
    PROPERTIES
    LABELS BigIO
    )
  set_tests_properties(
    VisibleHumanPasteWriteTest
    PROPERTIES
    DEPENDS VisibleHumanStreamReadWriteTest
    )
endif()
