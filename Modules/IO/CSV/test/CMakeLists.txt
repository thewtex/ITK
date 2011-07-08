itk_module_test()
set(ITK-IO-CSVTests
itkCSVArray2DFileReaderTest.cxx
itkCSVArray2DFileReaderWriterTest.cxx
itkCSVNumericObjectFileWriterTest.cxx
)

set(TEMP ${ITK_TEST_OUTPUT_DIR})

CreateTestDriver(ITK-IO-CSV  "${ITK-IO-CSV-Test_LIBRARIES}" "${ITK-IO-CSVTests}")

itk_add_test(NAME itkCSVArray2DFileReaderTest
      COMMAND ITK-IO-CSVTestDriver itkCSVArray2DFileReaderTest
              DATA{Input/csvFileReaderTestInput.csv} )
itk_add_test(NAME itkCSVNumericObjectFileWriterTest
      COMMAND ITK-IO-CSVTestDriver itkCSVNumericObjectFileWriterTest
              ${TEMP}/NumericObjectToCSVFileWriterOutput.csv)
itk_add_test(NAME itkCSVArray2DFileReaderWriterTest
      COMMAND ITK-IO-CSVTestDriver itkCSVArray2DFileReaderWriterTest
              ${TEMP}/csvFileArray2DReaderWriterTestOutput.csv)
