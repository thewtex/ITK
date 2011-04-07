# This project builds the test directories from all ITK modules as a separate
# project outside the main ITK build tree as if they were an application.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITKTestExternal)
if(ITK_SOURCE_DIR OR ITK_BINARY_DIR)
  message(FATAL_ERROR "This directory may build only outside ITK!")
endif()

include(CTest)

# Find the top of the main ITK source tree.
get_filename_component(ITK_TOP_DIR ${ITKTestExternal_SOURCE_DIR}/../.. ABSOLUTE)

# Load module infrastructure macros.
include(${ITK_TOP_DIR}/CMake/ITKModuleMacros.cmake)

# Find the ITK build or install tree.  Assume the version matches exactly.
# One should provide ITK_DIR explicitly in our intended use cases.
find_package(ITK REQUIRED)

# Use ITK without automatic IO factory registration.
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER 1)
include(${ITK_USE_FILE})

# Glob the set modules in the source tree.
file(GLOB meta RELATIVE "${ITK_TOP_DIR}"
   "${ITK_TOP_DIR}/*/*/*/itk-module.cmake" # grouped modules
  )
# Identify the subset of modules that have tests.
foreach(f ${meta})
  get_filename_component(_base ${f} PATH)
  if(EXISTS ${ITK_TOP_DIR}/${_base}/test)
    include(${ITK_TOP_DIR}/${f})
    set(${itk-module}_TEST_DIR ${_base}/test)
    unset(itk-module)
  endif()
endforeach()

# Input information for test build files.
set(ITK_DATA_ROOT ${ITK_TOP_DIR}/Testing/Data)
set(ITK_EXAMPLE_DATA_ROOT ${ITK_TOP_DIR}/Examples/Data)
set(ITK_TEST_OUTPUT_DIR "${ITKTestExternal_BINARY_DIR}/Testing/Temporary")
include(${ITK_TOP_DIR}/Modules/Core/TestKernel/CreateTestDriver.cmake)

# Add the test directory for each enabled module that has tests.
foreach(itk-module ${ITK_MODULES_ENABLED})
  if(${itk-module}_TEST_DIR)
    add_subdirectory(${ITK_TOP_DIR}/${${itk-module}_TEST_DIR}
                     ${ITKTestExternal_BINARY_DIR}/${${itk-module}_TEST_DIR})
  endif()
endforeach()
