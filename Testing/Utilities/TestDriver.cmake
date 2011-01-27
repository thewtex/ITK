#
# This include file creates the source code for a test driver.
# Two variables must be defined before including this file.
#   KIT should define a unique name for the test driver.
#   KitTests should contain a list of test file names without a .cxx suffix.
# The result is a variable CXX_TESTS that defines the name of the test driver
# executable.

set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkv4TestMainAfter.inc\"")
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KitTests}
  EXTRA_INCLUDE itkv4TestMain.h
  FUNCTION ProcessArguments
  )
set(TestDriver ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)
