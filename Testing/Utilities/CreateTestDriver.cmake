#
# This include file creates the source code for a test driver.
# Two variables must be defined before including this file.
#   KIT should define a unique name for the test driver.
#   KitTests should contain a list of test file names without a .cxx suffix.
# The result is a variable CXX_TESTS that defines the name of the test driver
# executable.

macro(CreateTestDriver KIT KIT_LIBS KitTests TestDriver)
  set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "#include \"itkTestDriverBeforeTest.inc\"")
  set(CMAKE_TESTDRIVER_AFTER_TESTMAIN "#include \"itkTestDriverAfterTest.inc\"")
  create_test_sourcelist(Tests ${KIT}CxxTests.cxx
    ${KitTests}
    EXTRA_INCLUDE itkTestDriverInclude.h
    FUNCTION ProcessArguments
    )
  add_executable(${KIT}CxxTests ${KIT}CxxTests.cxx ${Tests})
  target_link_libraries(${KIT}CxxTests ${KIT_LIBS})
  set(${TestDriver} ${EXECUTABLE_OUTPUT_PATH}/${KIT}CxxTests)
endmacro(CreateTestDriver)
