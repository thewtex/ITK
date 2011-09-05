cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if( "${CMAKE_GENERATOR}" MATCHES "Visual Studio 10" )
  cmake_minimum_required(VERSION 2.8.4)
endif()

project(ITK)

if( WIN32 )

  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "Source tree directory of ITK is too deep (${n} > 50)."
      "Please use a shorter root directory for the ITK source tree."
      )
  endif()

  string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "Binary tree directory of ITK is too deep (${n} > 50)."
      "Please use a shorter root directory for the ITK binary tree."
      )
  endif()

endif()

set(CMAKE_MODULE_PATH ${ITK_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(PreventInSourceBuilds)
include(PreventInBuildInstalls)
include(ITKModuleMacros)
include(ITKExternalData)
include(itkCheckSourceTree)

set(main_project_name ${_ITKModuleMacros_DEFAULT_LABEL})

#-----------------------------------------------------------------------------
# ITK version number.  An even minor number corresponds to releases.
set(ITK_VERSION_MAJOR "4")
set(ITK_VERSION_MINOR "0")
set(ITK_VERSION_PATCH "0")

if(NOT ITK_INSTALL_RUNTIME_DIR)
  set(ITK_INSTALL_RUNTIME_DIR bin)
endif()
if(NOT ITK_INSTALL_LIBRARY_DIR)
  set(ITK_INSTALL_LIBRARY_DIR lib)
endif()
if(NOT ITK_INSTALL_ARCHIVE_DIR)
  set(ITK_INSTALL_ARCHIVE_DIR lib)
endif()
if(NOT ITK_INSTALL_INCLUDE_DIR)
  set(ITK_INSTALL_INCLUDE_DIR include/ITK-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
endif()
if(NOT ITK_INSTALL_DATA_DIR)
  set(ITK_INSTALL_DATA_DIR share/ITK-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
endif()
if(NOT ITK_INSTALL_PACKAGE_DIR)
  set(ITK_INSTALL_PACKAGE_DIR "lib/cmake/ITK-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}")
endif()

# Override CMake's built-in add_* commands: assign LABELS to tests and targets
# automatically. Depends on the CMake variable itk-module being set to the
# "current" module when add_* is called.
macro(verify_itk_module_is_set)
  if("" STREQUAL "${itk-module}")
    message(FATAL_ERROR "CMake variable itk-module is not set")
  endif()
endmacro()

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


#-----------------------------------------------------------------------------
# Enable runinng cppcheck for each module on its source and test directories.
option(ITK_CPPCHECK_TEST "Run cppcheck for static code analysis" OFF)
mark_as_advanced(ITK_CPPCHECK_TEST)

#-----------------------------------------------------------------------------
# Default values for CMake options may vary based on whether it's a
# dashboard build or not:
#
if("$ENV{DASHBOARD_TEST_FROM_CTEST}" STREQUAL "")
  # developer build
  set(ITK_BUILD_ALL_MODULES_DEFAULT ON)
  set(BUILD_EXAMPLES_DEFAULT ON)
else()
  # dashboard build
  set(ITK_BUILD_ALL_MODULES_DEFAULT ON)
  set(BUILD_EXAMPLES_DEFAULT ON)
endif()

#-----------------------------------------------------------------------------
# Enable the download and use of BrainWeb datasets.
# When this data is available, additional 3D tests are enabled.
option(ITK_USE_BRAINWEB_DATA "Download and use BrainWeb data for advanced testing" OFF)
mark_as_advanced(ITK_USE_BRAINWEB_DATA)

#-----------------------------------------------------------------------------
# ITK wrapper for add_test that automatically sets the test's LABELS property
# to the value of its containing module.
#
function(itk_add_test)
  # Add tests with data in the ITKData group.
  ExternalData_add_test(ITKData ${ARGN})

  if("NAME" STREQUAL "${ARGV0}")
    set(_iat_testname ${ARGV1})
  else()
    set(_iat_testname ${ARGV0})
  endif()

  if(itk-module)
    set(_label ${itk-module})
    if(TARGET ${itk-module}-all AND "${ARGN}" MATCHES "DATA{")
      add_dependencies(${itk-module}-all ITKData)
    endif()
  else()
    set(_label ${main_project_name})
  endif()

  set_property(TEST ${_iat_testname} PROPERTY LABELS ${_label})
endfunction()

#-----------------------------------------------------------------------------
# ITK build configuration options.
option(BUILD_SHARED_LIBS "Build ITK with shared libraries." OFF)
set(ITK_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

include(ITKSetStandardCompilerFlags)
#---------------------------------------------------------------
# run try compiles and tests for ITK
include(itkTestFriendTemplatedFunction)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ITK_REQUIRED_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ITK_REQUIRED_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ITK_REQUIRED_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ITK_REQUIRED_LINK_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${ITK_REQUIRED_LINK_FLAGS}")

include(CTest)

include( CppcheckTargets )

# Setup build locations.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ITK_BINARY_DIR}/bin)
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ITK_BINARY_DIR}/lib)
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ITK_BINARY_DIR}/lib)
endif()
set(ITK_MODULES_DIR "${ITK_BINARY_DIR}/${ITK_INSTALL_PACKAGE_DIR}/Modules")


#-----------------------------------------------------------------------------
# Provide compatibility options.
option(ITKV3_COMPATIBILITY "Enable compatibility with ITK3.x when possible." OFF)
option(ITK_LEGACY_REMOVE "Remove all legacy code completely." OFF)
option(ITK_LEGACY_SILENT "Silence all legacy code messages." OFF)
mark_as_advanced(ITK_LEGACY_REMOVE ITK_LEGACY_SILENT ITK_V3_COMPATIBILITY)

#-----------------------------------------------------------------------------
# ITK build classes that are in the review process
option(ITK_USE_REVIEW "Build algorithms that are candidates to be included in the Toolkit. This classes are not covered by the Backward Compatibility Policy, no
r the Copyright Policy of ITK." OFF)
mark_as_advanced(ITK_USE_REVIEW)

if (ITK_USE_REVIEW)
  set(Module_ITKReview ON)
endif(ITK_USE_REVIEW)

#-----------------------------------------------------------------------------
# ITK uses KWStyle for checking the coding style
INCLUDE(${ITK_SOURCE_DIR}/Utilities/KWStyle/KWStyle.cmake)

#-----------------------------------------------------------------------------
# ITK build the Examples that are illustrated in the Software Guide
option(BUILD_EXAMPLES "Build the Examples directory."
  ${BUILD_EXAMPLES_DEFAULT})

#-----------------------------------------------------------------------------
# Enable GPU support. Requires OpenCL to be installed
option(ITK_USE_GPU "GPU acceleration via OpenCL" OFF)
mark_as_advanced(ITK_USE_GPU)

# Manage FFT v3 Options
#
# USE_FFTWD -- use double precision fftw
option(USE_FFTWD "Use double precision fftw if found" OFF)
mark_as_advanced(USE_FFTWD)
#
# USE_FFTWF -- use single precision fftw
option(USE_FFTWF "Use single precision fftw if found" OFF)
mark_as_advanced(USE_FFTWF)

# USE_SYSTEM_FFTW -- locate a readybuilt fftw installation
option(USE_SYSTEM_FFTW "Use an installed version of fftw" OFF)
mark_as_advanced(USE_SYSTEM_FFTW)

if( USE_FFTWD OR USE_FFTWF )
  #
  # use ExternalProject
  include(ExternalProject)

  include(itkExternal_FFTW)
endif( USE_FFTWD OR USE_FFTWF )

if(NOT ITKV3_COMPATIBILITY)
  option(ITK_USE_64BITS_IDS "When ON, ITK will use 64 bits integers to index pixels. This is needed for managing images larger than 4Gb in some platforms." OFF)
  mark_as_advanced(ITK_USE_64BITS_IDS)
endif()

# ITK turn on concept checking
option(ITK_USE_CONCEPT_CHECKING "Turn on concept checking to give helpful errors at compile time if a type cannot be used as a template parameter." ON)
mark_as_advanced(ITK_USE_CONCEPT_CHECKING)
option(ITK_USE_STRICT_CONCEPT_CHECKING "Turn on Strict concept checking to give more stringent errors at compile time if a type cannot be used as a template parameter." OFF)
mark_as_advanced(ITK_USE_STRICT_CONCEPT_CHECKING)

#----------------------------------------------------------------------------
set(ITK_TEST_OUTPUT_DIR "${ITK_BINARY_DIR}/Testing/Temporary")

#-----------------------------------------------------------------------------
# Configure the default ITK_DATA_ROOT for the location of ITK Data.
unset(ITK_DATA_ROOT CACHE) # No longer a cache entry.
if(EXISTS ${ITK_SOURCE_DIR}/Testing/Data/ITKData.readme)
  set(ITK_DATA_ROOT ${ITK_SOURCE_DIR}/Testing/Data)
else()
  set(ITK_DATA_ROOT "ITK_DATA_ROOT-NOTFOUND")
  foreach(opt BUILD_TESTING BUILD_EXAMPLES)
    if(${opt})
      message(FATAL_ERROR
        "${opt} is ON but the Testing/Data submodule is not available.  "
        "Run\n  git submodule update --init\n"
        "from the top of the source tree to get it!")
    endif()
  endforeach()
endif()

# Location of ITK Example Data.
set(ITK_EXAMPLE_DATA_ROOT "${ITK_SOURCE_DIR}/Examples/Data")

#This flag is used in particular, to enable some tests that require large memory to run.
# This probably can be discovered at configuration time by CMake. (Check with CMake developers).
set(ITK_COMPUTER_MEMORY_SIZE 1 CACHE STRING "Provide here the size of your RAM Memory in GigaBytes")
mark_as_advanced(ITK_COMPUTER_MEMORY_SIZE)




#----------------------------------------------------------------------
# Load the module DAG.
set(ITK_MODULES_ALL)
file(GLOB meta RELATIVE "${ITK_SOURCE_DIR}"
   "${ITK_SOURCE_DIR}/*/*/*/itk-module.cmake" # grouped modules
  )
foreach(f ${meta})
  include(${ITK_SOURCE_DIR}/${f})
  list(APPEND ITK_MODULES_ALL ${itk-module})
  get_filename_component(${itk-module}_BASE ${f} PATH)
  set(${itk-module}_SOURCE_DIR ${ITK_SOURCE_DIR}/${${itk-module}_BASE})
  set(${itk-module}_BINARY_DIR ${ITK_BINARY_DIR}/${${itk-module}_BASE})
  if(BUILD_TESTING AND EXISTS ${${itk-module}_SOURCE_DIR}/test)
    list(APPEND ITK_MODULES_ALL ${itk-module-test})
    set(${itk-module-test}_SOURCE_DIR ${${itk-module}_SOURCE_DIR}/test)
    set(${itk-module-test}_BINARY_DIR ${${itk-module}_BINARY_DIR}/test)
    set(${itk-module-test}_IS_TEST 1)
    set(${itk-module}_TESTED_BY ${itk-module-test})
    set(${itk-module-test}_TESTS_FOR ${itk-module})
  endif()

  # Reject bad dependencies.
  string(REGEX MATCHALL ";(ITKDeprecated|ITKReview|ITKIntegratedTest);"
    _bad_deps ";${ITK_MODULE_${itk-module}_DEPENDS};${ITK_MODULE_${itk-module-test}_DEPENDS};")
  foreach(dep ${_bad_deps})
    if(NOT "${itk-module}" MATCHES "^(${dep}|ITKIntegratedTest)$")
      message(FATAL_ERROR
        "Module \"${itk-module}\" loaded from\n"
        "  ${${itk-module}_BASE}/itk-module.cmake\n"
        "may not depend on module \"${dep}\".")
    endif()
  endforeach()
endforeach()
# Clear variables set later in each module.
unset(itk-module)
unset(itk-module-test)

# Validate the module DAG.
macro(itk_module_check itk-module _needed_by stack)
  if(NOT ITK_MODULE_${itk-module}_DECLARED)
    message(FATAL_ERROR "No such module \"${itk-module}\" needed by \"${_needed_by}\"")
  endif()
  if(check_started_${itk-module} AND NOT check_finished_${itk-module})
    # We reached a module while traversing its own dependencies recursively.
    set(msg "")
    foreach(entry ${stack})
      set(msg " ${entry} =>${msg}")
      if("${entry}" STREQUAL "${itk-module}")
        break()
      endif()
    endforeach()
    message(FATAL_ERROR "Module dependency cycle detected:\n ${msg} ${itk-module}")
  elseif(NOT check_started_${itk-module})
    # Traverse dependencies of this module.  Mark the start and finish.
    set(check_started_${itk-module} 1)
    foreach(dep IN LISTS ITK_MODULE_${itk-module}_DEPENDS)
      itk_module_check(${dep} ${itk-module} "${itk-module};${stack}")
    endforeach()
    set(check_finished_${itk-module} 1)
  endif()
endmacro()

foreach(itk-module ${ITK_MODULES_ALL})
  itk_module_check("${itk-module}" "" "")
endforeach()

#----------------------------------------------------------------------

# Provide an option for all modules.
option(ITK_BUILD_ALL_MODULES "Request to build all modules"
  ${ITK_BUILD_ALL_MODULES_DEFAULT})

# Provide module selections by groups
include(${ITK_SOURCE_DIR}/CMake/ITKGroups.cmake)

# Provide an option for each module.
foreach(itk-module ${ITK_MODULES_ALL})
  if(NOT ${itk-module}_IS_TEST)
    option(Module_${itk-module} "Request building ${itk-module}" OFF)
    mark_as_advanced(Module_${itk-module})
    if(ITK_MODULE_${itk-module}_EXCLUDE_FROM_ALL)
      set(ITK_MODULE_${itk-module}_IN_ALL 0)
    else()
      set(ITK_MODULE_${itk-module}_IN_ALL ${ITK_BUILD_ALL_MODULES})
    endif()
  endif()
endforeach()

# Follow dependencies.
macro(itk_module_enable itk-module _needed_by)
  if(NOT Module_${itk-module})
    if(NOT ${itk-module}_TESTED_BY OR
      NOT "x${_needed_by}" STREQUAL "x${${itk-module}_TESTED_BY}")
      list(APPEND ITK_MODULE_${itk-module}_NEEDED_BY ${_needed_by})
    endif()
  endif()
  if(NOT ${itk-module}_ENABLED)
    set(${itk-module}_ENABLED 1)
    foreach(dep IN LISTS ITK_MODULE_${itk-module}_DEPENDS)
      itk_module_enable(${dep} ${itk-module})
    endforeach()
    if(${itk-module}_TESTED_BY)
      itk_module_enable(${${itk-module}_TESTED_BY} "")
    endif()
  endif()
endmacro()

foreach(itk-module ${ITK_MODULES_ALL})
  if(Module_${itk-module} OR ITK_MODULE_${itk-module}_IN_ALL)
    itk_module_enable("${itk-module}" "")
  elseif(ITK_MODULE_${itk-module}_REQUEST_BY)
    itk_module_enable("${itk-module}" "${ITK_MODULE_${itk-module}_REQUEST_BY}")
  endif()
endforeach()

# Build final list of enabled modules.
set(ITK_MODULES_ENABLED "")
set(ITK_MODULES_DISABLED "")
foreach(itk-module ${ITK_MODULES_ALL})
  if(${itk-module}_ENABLED)
    list(APPEND ITK_MODULES_ENABLED ${itk-module})
  else()
    list(APPEND ITK_MODULES_DISABLED ${itk-module})
  endif()
endforeach()
list(SORT ITK_MODULES_ENABLED) # Deterministic order.
list(SORT ITK_MODULES_DISABLED) # Deterministic order.

# Order list to satisfy dependencies.
include(CMake/TopologicalSort.cmake)
topological_sort(ITK_MODULES_ENABLED ITK_MODULE_ _DEPENDS)

set(ITK_MODULES_DISABLED_CPACK )
foreach(m ${ITK_MODULES_DISABLED})
  list(APPEND ITK_MODULES_DISABLED_CPACK "/${m}/")
endforeach()
set(CPACK_SOURCE_IGNORE_FILES
  "${ITK_MODULES_DISABLED_CPACK};/\\\\.git")
include(CPack)

# Report what will be built.
foreach(itk-module ${ITK_MODULES_ENABLED})
  if(NOT ${itk-module}_IS_TEST)
    if(Module_${itk-module})
      set(_reason ", requested by Module_${itk-module}")
    elseif(ITK_MODULE_${itk-module}_IN_ALL)
      set(_reason ", requested by ITK_BUILD_ALL_MODULES")
    else()
      set(_reason ", needed by [${ITK_MODULE_${itk-module}_NEEDED_BY}]")
    endif()
    message(STATUS "Enabled ${itk-module}${_reason}.")
  endif()
endforeach()

# Hide options for modules that will build anyway.
foreach(itk-module ${ITK_MODULES_ALL})
  if(NOT ${itk-module}_IS_TEST)
    if(ITK_MODULE_${itk-module}_IN_ALL OR ITK_MODULE_${itk-module}_NEEDED_BY)
      set_property(CACHE Module_${itk-module} PROPERTY TYPE INTERNAL)
    else()
      set_property(CACHE Module_${itk-module} PROPERTY TYPE BOOL)
    endif()
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Construct an in-build-order list of "CDash subproject" modules from the
# list of enabled modules:

set(ITK_CDASH_SUBPROJECT_MODULES)

if(ITK_GENERATE_PROJECT_XML OR ITK_GENERATE_SUBPROJECTS_CMAKE)
  # Since a "CDash subproject" for ITK logically contains both a non-test
  # module and its corresponding test module, the subproject order must take
  # both modules into account. The subprojects in this list are named after
  # the non-test modules in the enabled modules list. But the ordering of
  # these subprojects use the dependencies of the modules *and* their test
  # modules
  foreach(module ${ITK_MODULES_ENABLED})
    if(${module}_TESTED_BY)
      # module that HAS a test module: skip for now... will be added as a
      # subproject later, in the slot when its corresponding "-test" module
      # is encountered
    elseif(${module}_TESTS_FOR)
      # this is a test module... *now* list the module which it tests as
      # the subproject name:
      list(APPEND ITK_CDASH_SUBPROJECT_MODULES ${${module}_TESTS_FOR})
    else()
      # a module that is not a test module, and has no tested by...
      # is just a module-only subproject with no test module:
      list(APPEND ITK_CDASH_SUBPROJECT_MODULES ${module})
    endif()
  endforeach()
endif()

#-----------------------------------------------------------------------------
# Write a Project.xml file to send the description of the submodules and
# their dependencies up to CDash:

if(ITK_GENERATE_PROJECT_XML)
  set(filename "${ITK_BINARY_DIR}/${main_project_name}.Project.xml")

  set(xml "<?xml version='1.0' encoding='UTF-8'?>\n")
  set(xml "${xml}<Project name='${main_project_name}'>\n")
  foreach(module ${ITK_CDASH_SUBPROJECT_MODULES})
    if(${module}_IS_TEST)
      message(FATAL_ERROR "unexpected: subproject names should not be test modules module='${module}' tests_for='${${module}_TESTS_FOR}'")
    endif()
    set(xml "${xml}  <SubProject name='${module}'>\n")
    set(deps "")
    set(dep_list ${ITK_MODULE_${module}_DEPENDS})
    if(${module}_TESTED_BY)
      list(APPEND dep_list ${ITK_MODULE_${${module}_TESTED_BY}_DEPENDS})
      if(dep_list)
        list(SORT dep_list)
      endif()
    endif()
    foreach(dep ${dep_list})
      if(NOT ${dep}_IS_TEST AND NOT "${module}" STREQUAL "${dep}")
        set(xml "${xml}    <Dependency name='${dep}'/>\n")
      endif()
    endforeach()
    set(xml "${xml}  </SubProject>\n")
  endforeach()
  set(xml "${xml}</Project>\n")

  # Always write out "${filename}.in":
  file(WRITE ${filename}.in "${xml}")

  # Use configure_file so "${filename}" only changes when its content changes:
  configure_file(${filename}.in ${filename} COPYONLY)
endif()

#-----------------------------------------------------------------------------
# Write the list of enabled modules out for ctest scripts to use as an
# in-order subproject list:

if(ITK_GENERATE_SUBPROJECTS_CMAKE)
  set(filename "${ITK_BINARY_DIR}/${main_project_name}.SubProjects.cmake")

  set(s "# Generated by CMake, do not edit!\n")
  set(s "${s}set(itk_subprojects\n")
  foreach(itk-module ${ITK_CDASH_SUBPROJECT_MODULES})
    if(${itk-module}_IS_TEST)
      message(FATAL_ERROR "unexpected: subproject names should not be test modules itk-module='${itk-module}' tests_for='${${itk-module}_TESTS_FOR}'")
    endif()
    set(s "${s}  \"${itk-module}\"\n")
  endforeach()
  set(s "${s})\n")

  # Always write out "${filename}.in":
  file(WRITE ${filename}.in "${s}")

  # Use configure_file so "${filename}" only changes when its content changes:
 configure_file(${filename}.in ${filename} COPYONLY)
endif()

#-----------------------------------------------------------------------------

if(NOT ITK_MODULES_ENABLED)
  message(WARNING "No modules enabled!")
  file(REMOVE "${ITK_BINARY_DIR}/ITKTargets.cmake")
  return()
endif()

file(WRITE "${ITK_BINARY_DIR}/ITKTargets.cmake"
  "# Generated by CMake, do not edit!")

macro(init_module_vars)
  verify_itk_module_is_set()
  set(${itk-module}-targets ITKTargets)
  set(${itk-module}-targets-install "${ITK_INSTALL_PACKAGE_DIR}/ITKTargets.cmake")
  set(${itk-module}-targets-build "${ITK_BINARY_DIR}/ITKTargets.cmake")
endmacro()

# Build all modules.
foreach(itk-module ${ITK_MODULES_ENABLED})
  if(NOT ${itk-module}_IS_TEST)
    init_module_vars()
  endif()
  include("${${itk-module}_SOURCE_DIR}/itk-module-init.cmake" OPTIONAL)
  add_subdirectory("${${itk-module}_SOURCE_DIR}" "${${itk-module}_BINARY_DIR}")
endforeach()

#----------------------------------------------------------------------------
configure_file(CMake/CTestCustom.cmake.in CTestCustom.cmake)

#-----------------------------------------------------------------------------

# Create list of available modules and libraries.
set(ITK_CONFIG_MODULES_ENABLED "")
foreach(itk-module ${ITK_MODULES_ENABLED})
  if(NOT ${itk-module}_IS_TEST)
    list(APPEND ITK_CONFIG_MODULES_ENABLED ${itk-module})
  endif()
endforeach()

# Generate ITKConfig.cmake for the build tree.
set(ITK_CONFIG_CODE "
set(ITK_MODULES_DIR \"${ITK_MODULES_DIR}\")")
set(ITK_USE_FILE "${ITK_SOURCE_DIR}/CMake/UseITK.cmake")
set(ITK_CONFIG_TARGETS_CONDITION " AND NOT ITK_BINARY_DIR")
set(ITK_CONFIG_TARGETS_FILE "${ITK_BINARY_DIR}/ITKTargets.cmake")
set(ITK_CONFIG_MODULE_API_FILE "${ITK_SOURCE_DIR}/CMake/ITKModuleAPI.cmake")
configure_file(CMake/ITKConfig.cmake.in ITKConfig.cmake @ONLY)

# Generate ITKConfig.cmake for the install tree.
set(ITK_CONFIG_CODE "
# Compute the installation prefix from this ITKConfig.cmake file location.
get_filename_component(ITK_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
# Construct the proper number of get_filename_component(... PATH)
# calls to compute the installation prefix.
string(REGEX REPLACE "/" ";" _count "${ITK_INSTALL_PACKAGE_DIR}")
foreach(p ${_count})
  set(ITK_CONFIG_CODE "${ITK_CONFIG_CODE}
get_filename_component(ITK_INSTALL_PREFIX \"\${ITK_INSTALL_PREFIX}\" PATH)")
endforeach(p)
set(ITK_CONFIG_CODE "${ITK_CONFIG_CODE}
set(ITK_MODULES_DIR \"\${ITK_INSTALL_PREFIX}/${ITK_INSTALL_PACKAGE_DIR}/Modules\")")
set(ITK_USE_FILE "\${ITK_INSTALL_PREFIX}/${ITK_INSTALL_PACKAGE_DIR}/UseITK.cmake")
set(ITK_CONFIG_TARGETS_CONDITION "")
set(ITK_CONFIG_TARGETS_FILE "\${ITK_INSTALL_PREFIX}/${ITK_INSTALL_PACKAGE_DIR}/ITKTargets.cmake")
set(ITK_CONFIG_MODULE_API_FILE "\${ITK_INSTALL_PREFIX}/${ITK_INSTALL_PACKAGE_DIR}/ITKModuleAPI.cmake")
configure_file(CMake/ITKConfig.cmake.in CMakeFiles/ITKConfig.cmake @ONLY)

#----------------------------------------------------------------------------
# Configure maintenance scripts
configure_file(Utilities/Maintenance/doSingleKWStyleUncrustifyFix.sh.in
  Utilities/Maintenance/doSingleKWStyleUncrustifyFix.sh @ONLY)

#-----------------------------------------------------------------------------
configure_file(CMake/ITKConfigVersion.cmake.in ITKConfigVersion.cmake @ONLY)

install(FILES ${ITK_BINARY_DIR}/CMakeFiles/ITKConfig.cmake
              ${ITK_BINARY_DIR}/ITKConfigVersion.cmake
              CMake/ITKModuleAPI.cmake
              CMake/UseITK.cmake
              CMake/itkImageIOFactoryRegisterManager.h.in
  DESTINATION ${ITK_INSTALL_PACKAGE_DIR})
get_property(ITKTargets_MODULES GLOBAL PROPERTY ITKTargets_MODULES)
if(ITKTargets_MODULES)
  install(EXPORT ITKTargets DESTINATION ${ITK_INSTALL_PACKAGE_DIR})
else()
  set(CMAKE_CONFIGURABLE_FILE_CONTENT "# No targets!")
  configure_file(${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in
                 ${ITK_BINARY_DIR}/CMakeFiles/ITKTargets.cmake @ONLY)
  install(FILES ${ITK_BINARY_DIR}/CMakeFiles/ITKTargets.cmake
          DESTINATION ${ITK_INSTALL_PACKAGE_DIR})
endif()

if(BUILD_TESTING)
  add_subdirectory(Utilities/InstallTest)
endif()

#-----------------------------------------------------------------------------
# The subdirectories added below this line should use only the public
# interface with find_package(ITK).  Set ITK_DIR to use this ITK build.
set(ITK_DIR "${ITK_BINARY_DIR}")

include(Wrapping/itkWrapSetup.cmake)
add_subdirectory(Wrapping)

if(BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

#----------------------------------------------------------------------
# Provide an option for generating documentation.
add_subdirectory(Utilities/Doxygen)

# Create target to download data from the ITKData group.  This must come after
# all tests have been added that reference the group, so we put it last.
ExternalData_Add_Target(ITKData)

set(OLD_DATA_FILES
Testing/Data/Baseline/Algorithms/BinaryThinningImageFilterTest.png
Testing/Data/Baseline/Algorithms/IsolatedWatershedImageFilterTest.png
Testing/Data/Baseline/Algorithms/OtsuMultipleThresholdsImageFilterTest.png
Testing/Data/Baseline/Algorithms/OtsuThresholdImageFilterTest.png
Testing/Data/Baseline/Algorithms/STAPLEImageFilterTest.mha
Testing/Data/Baseline/Algorithms/VectorThresholdSegmentationLevelSetImageFilterTest.1.png
Testing/Data/Baseline/Algorithms/VectorThresholdSegmentationLevelSetImageFilterTest.2.png
Testing/Data/Baseline/Algorithms/VectorThresholdSegmentationLevelSetImageFilterTest.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.1.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.2.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.3.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.4.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.5.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest1.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest2.1.png
Testing/Data/Baseline/Algorithms/VoronoiPartioningImageFilterTest2.png
Testing/Data/Baseline/Algorithms/itkBayesianClassifierImageFilterTestLabelMap.png
Testing/Data/Baseline/Algorithms/itkCurvatureFlowTestPython2.png
Testing/Data/Baseline/Algorithms/itkCurvatureFlowTestTcl.png
Testing/Data/Baseline/Algorithms/itkCurvatureFlowTestTcl2.png
Testing/Data/Baseline/Algorithms/itkMultiResolutionPDEDeformableRegistrationTest.1.png
Testing/Data/Baseline/Algorithms/itkMultiResolutionPDEDeformableRegistrationTest.png
Testing/Data/Baseline/Algorithms/itkMultiResolutionPDEDeformableRegistrationTestPixelCentered.png
Testing/Data/Baseline/Algorithms/itkNarrowBandCurvesLevelSetImageFilterTest.png
Testing/Data/Baseline/Algorithms/itkThresholdMaximumConnectedComponentsImageFilterTest1.png
Testing/Data/Baseline/Algorithms/itkThresholdMaximumConnectedComponentsImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/AccumulateImageFilterTest.png
Testing/Data/Baseline/BasicFilters/AdaptiveHistogramEqualizationImageFilterTest.png
Testing/Data/Baseline/BasicFilters/AdaptiveHistogramEqualizationImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/BilateralImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/BilateralImageFilterTest3.png
Testing/Data/Baseline/BasicFilters/BilateralImageFilterTest4.png
Testing/Data/Baseline/BasicFilters/BinaryThresholdImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/ClosingByReconstructionImageFilterTest.png
Testing/Data/Baseline/BasicFilters/ClosingByReconstructionImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/ConfidenceConnectedImageFilterTest.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest.1.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest2.1.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest3.1.png
Testing/Data/Baseline/BasicFilters/ConnectedComponentImageFilterTest3.png
Testing/Data/Baseline/BasicFilters/ConnectedThresholdImageFilterTest.png
Testing/Data/Baseline/BasicFilters/ConnectedThresholdImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/DiscreteGaussianImageFilterTest2_OutputA.mha
Testing/Data/Baseline/BasicFilters/DoubleThresholdImageFilterTest.png
Testing/Data/Baseline/BasicFilters/DoubleThresholdImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/ExtractOrthogonalSwath2DImageFilterTest.png
Testing/Data/Baseline/BasicFilters/GradientAnisotropicDiffusionImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/GrayscaleConnectedClosingImageFilterTest.png
Testing/Data/Baseline/BasicFilters/GrayscaleConnectedClosingImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/GrayscaleConnectedOpeningImageFilterTest.png
Testing/Data/Baseline/BasicFilters/GrayscaleConnectedOpeningImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/GrayscaleFillholeImageFilterTest.png
Testing/Data/Baseline/BasicFilters/GrayscaleMorphologicalClosingImageFilterTest.png
Testing/Data/Baseline/BasicFilters/GrayscaleMorphologicalOpeningImageFilterTest.png
Testing/Data/Baseline/BasicFilters/HConvexConcaveImageFilterTest.png
Testing/Data/Baseline/BasicFilters/HMaximaMinimaImageFilterTest.png
Testing/Data/Baseline/BasicFilters/HMaximaMinimaImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/HeadMRVolumeBinaryProjection100.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeBinaryProjection200.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeBinaryThresholdProjection.png
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMaximumProjection.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMaximumProjection0.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMaximumProjection1.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMaximumProjection2D0.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMaximumProjection2D1.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMeanProjection.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMedianProjection.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeMinimumProjection.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeStandardDeviationProjection.tif
Testing/Data/Baseline/BasicFilters/HeadMRVolumeSumProjection.tif
Testing/Data/Baseline/BasicFilters/ImageToVectorImageFilterTest.png
Testing/Data/Baseline/BasicFilters/InvertIntensityImageFilterTest.png
Testing/Data/Baseline/BasicFilters/IsolatedConnectedImageFilterTest.png
Testing/Data/Baseline/BasicFilters/IsolatedConnectedImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/LaplacianRecursiveGaussianImageFilterTest.1.png
Testing/Data/Baseline/BasicFilters/LaplacianRecursiveGaussianImageFilterTest.2.png
Testing/Data/Baseline/BasicFilters/LaplacianRecursiveGaussianImageFilterTest.png
Testing/Data/Baseline/BasicFilters/MaskConnectedComponentImageFilterTest.png
Testing/Data/Baseline/BasicFilters/MaskNeighborhoodOperatorImageFilterTest.png
Testing/Data/Baseline/BasicFilters/MatrixIndexSelectionImageFilterTest.png
Testing/Data/Baseline/BasicFilters/ModulusImageFilterTest.png
Testing/Data/Baseline/BasicFilters/MorphologicalGradientImageFilterTest.png
Testing/Data/Baseline/BasicFilters/NeighborhoodConnectedImageFilterTest.png
Testing/Data/Baseline/BasicFilters/NormalizedCorrelationImageFilterTest.png
Testing/Data/Baseline/BasicFilters/OpeningByReconstructionImageFilterTest.png
Testing/Data/Baseline/BasicFilters/OpeningByReconstructionImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/ParallelSparseFieldLevelSetImageFilterTest.mha
Testing/Data/Baseline/BasicFilters/PasteImageFilterTest.png
Testing/Data/Baseline/BasicFilters/PromoteDimensionImageTest.mhd
Testing/Data/Baseline/BasicFilters/PromoteDimensionImageTest.raw
Testing/Data/Baseline/BasicFilters/PushPopTileImageFilterTest.png
Testing/Data/Baseline/BasicFilters/RelabelComponentImageFilterTest.png
Testing/Data/Baseline/BasicFilters/RemoveBoundaryObjectsTest.png
Testing/Data/Baseline/BasicFilters/RemoveBoundaryObjectsTest2.png
Testing/Data/Baseline/BasicFilters/ResampleImageTest2.png
Testing/Data/Baseline/BasicFilters/ScalarConnectedComponentImageFilterTest.png
Testing/Data/Baseline/BasicFilters/SimpleContourExtractorImageFilterTest.png
Testing/Data/Baseline/BasicFilters/TileImageFilterTest4.png
Testing/Data/Baseline/BasicFilters/TobogganImageFilterTest.png
Testing/Data/Baseline/BasicFilters/VectorConfidenceConnectedImageFilterTest.png
Testing/Data/Baseline/BasicFilters/VectorConnectedComponentImageFilterTest.png
Testing/Data/Baseline/BasicFilters/VectorGradientMagnitudeImageFilterTest1a.png
Testing/Data/Baseline/BasicFilters/VectorGradientMagnitudeImageFilterTest1b.png
Testing/Data/Baseline/BasicFilters/VectorGradientMagnitudeImageFilterTest2.png
Testing/Data/Baseline/BasicFilters/VectorGradientMagnitudeImageFilterTest2b.png
Testing/Data/Baseline/BasicFilters/VectorResampleImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkApproximateSignedDistanceMapImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-100-0-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-100-0-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-100-150-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-100-150-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-200-0-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-200-0-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-200-150-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryDilateImageFilterTest-200-150-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-100-0-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-100-0-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-100-150-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-100-150-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-200-0-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-200-0-1.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-200-150-0.png
Testing/Data/Baseline/BasicFilters/itkBinaryErodeImageFilterTest-200-150-1.png
Testing/Data/Baseline/BasicFilters/itkBlackTopHatImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkCannyEdgeDetectionImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkDerivativeImageFilterTest1x.png
Testing/Data/Baseline/BasicFilters/itkDerivativeImageFilterTest1y.png
Testing/Data/Baseline/BasicFilters/itkDerivativeImageFilterTest2x.png
Testing/Data/Baseline/BasicFilters/itkDerivativeImageFilterTest2y.png
Testing/Data/Baseline/BasicFilters/itkGrayscaleFunctionDilateImageFilterTest.mha
Testing/Data/Baseline/BasicFilters/itkGrayscaleFunctionErodeImageFilterTest.mha
Testing/Data/Baseline/BasicFilters/itkNarrowBandImageFilterBaseTest.png
Testing/Data/Baseline/BasicFilters/itkNoiseImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkPolylineMask2DImageFilterTest.png
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest1.mhd
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest1.zraw
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest2.mhd
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest2.zraw
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest3.mhd
Testing/Data/Baseline/BasicFilters/itkSignedMaurerDistanceMapImageFilterTest3.zraw
Testing/Data/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest1.mha
Testing/Data/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest2.mha
Testing/Data/Baseline/BasicFilters/itkVectorIndexSelectionCastImageFilterTest3.mha
Testing/Data/Baseline/BasicFilters/itkWhiteTopHatImageFilterTest.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest2.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest2PixelCentered.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest3.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest3PixelCentered.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest4.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest4PixelCentered.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest5.png
Testing/Data/Baseline/Common/itkBSplineDeformableTransformTest5PixelCentered.png
Testing/Data/Baseline/Common/itkDifferenceImageFilterTest.png
Testing/Data/Baseline/Common/itkLineIteratorTest.txt
Testing/Data/Baseline/Filtering/AntiAliasBinaryImageFilter.png
Testing/Data/Baseline/Filtering/BilateralImageFilter.1.png
Testing/Data/Baseline/Filtering/BilateralImageFilter.2.png
Testing/Data/Baseline/Filtering/BilateralImageFilter.png
Testing/Data/Baseline/Filtering/BinaryMedianImageFilter1.png
Testing/Data/Baseline/Filtering/BinaryMedianImageFilter2.png
Testing/Data/Baseline/Filtering/BinaryMedianImageFilter3.png
Testing/Data/Baseline/Filtering/BinaryMinMaxCurvatureFlowImageFilter.png
Testing/Data/Baseline/Filtering/BinaryThresholdImageFilter.png
Testing/Data/Baseline/Filtering/BinomialBlurImageFilter.png
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_multipleSeeds.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_multipleSeeds_NoHandlesTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_multipleSeeds_NoTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_multipleSeeds_StrictTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_multipleSeeds_Topo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_singleSeed.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_singleSeed_NoHandlesTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_singleSeed_NoTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_singleSeed_StrictTopo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_singleSeed_Topo_out.nii.gz
Testing/Data/Baseline/Filtering/BrainProtonDensitySlice_speed.nii.gz
Testing/Data/Baseline/Filtering/CannyEdgeDetectionImageFilter.png
Testing/Data/Baseline/Filtering/CannyEdgeDetectionImageFilter2.png
Testing/Data/Baseline/Filtering/CurvatureAnisotropicDiffusionImageFilter.1.png
Testing/Data/Baseline/Filtering/CurvatureAnisotropicDiffusionImageFilter.2.png
Testing/Data/Baseline/Filtering/CurvatureAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/CurvatureFlowImageFilter.png
Testing/Data/Baseline/Filtering/DanielssonDistanceMapImageFilter.png
Testing/Data/Baseline/Filtering/DerivativeImageFilter.mha
Testing/Data/Baseline/Filtering/DigitallyReconstructedRadiograph1.png
Testing/Data/Baseline/Filtering/DiscreteGaussianImageFilter.png
Testing/Data/Baseline/Filtering/FACorpusCallosum.mhd
Testing/Data/Baseline/Filtering/FACorpusCallosum.raw
Testing/Data/Baseline/Filtering/FFTDirectInverse2Point.png
Testing/Data/Baseline/Filtering/FFTDirectInversePoint.png
Testing/Data/Baseline/Filtering/FFTImageFilterReal.png
Testing/Data/Baseline/Filtering/FlipImageFilterX.png
Testing/Data/Baseline/Filtering/FlipImageFilterXY.png
Testing/Data/Baseline/Filtering/FlipImageFilterY.png
Testing/Data/Baseline/Filtering/GaussianBlurImageFunction.mha
Testing/Data/Baseline/Filtering/GradientAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/GradientMagnitudeImageFilter.png
Testing/Data/Baseline/Filtering/GradientMagnitudeRecursiveGaussianImageFilter.png
Testing/Data/Baseline/Filtering/LaplacianImageFilter.1.png
Testing/Data/Baseline/Filtering/LaplacianImageFilter.2.png
Testing/Data/Baseline/Filtering/LaplacianImageFilter.png
Testing/Data/Baseline/Filtering/LaplacianImageFilterPythonTest.png
Testing/Data/Baseline/Filtering/LaplacianRecursiveGaussianImageFilter1.mha
Testing/Data/Baseline/Filtering/LaplacianRecursiveGaussianImageFilter2.mha
Testing/Data/Baseline/Filtering/LaplacianSharpeningImageFilter.1.png
Testing/Data/Baseline/Filtering/LaplacianSharpeningImageFilter.2.png
Testing/Data/Baseline/Filtering/LaplacianSharpeningImageFilter.png
Testing/Data/Baseline/Filtering/MathematicalMorphologyBinaryFiltersDilate.png
Testing/Data/Baseline/Filtering/MathematicalMorphologyBinaryFiltersErode.png
Testing/Data/Baseline/Filtering/MathematicalMorphologyGrayscaleFiltersDilate.png
Testing/Data/Baseline/Filtering/MathematicalMorphologyGrayscaleFiltersErode.png
Testing/Data/Baseline/Filtering/MeanImageFilter.png
Testing/Data/Baseline/Filtering/MedianImageFilter.png
Testing/Data/Baseline/Filtering/MorphologicalImageEnhancement.png
Testing/Data/Baseline/Filtering/OtsuMultipleThresholdOutput002.png
Testing/Data/Baseline/Filtering/OtsuThresholdImageFilter.png
Testing/Data/Baseline/Filtering/RACorpusCallosum.mhd
Testing/Data/Baseline/Filtering/RACorpusCallosum.raw
Testing/Data/Baseline/Filtering/RGBCurvatureAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/RGBGradientAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/RGBToGrayscale.png
Testing/Data/Baseline/Filtering/ResampleImageFilter2Test1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter2Test2.png
Testing/Data/Baseline/Filtering/ResampleImageFilter2Test3.png
Testing/Data/Baseline/Filtering/ResampleImageFilter2Test4.png
Testing/Data/Baseline/Filtering/ResampleImageFilter2Test5.png
Testing/Data/Baseline/Filtering/ResampleImageFilter3Test1.1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter3Test1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter3Test1PixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter4Test.png
Testing/Data/Baseline/Filtering/ResampleImageFilter4TestPixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter5Test.1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter5Test.png
Testing/Data/Baseline/Filtering/ResampleImageFilter5TestPixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter6.png
Testing/Data/Baseline/Filtering/ResampleImageFilter6PixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter7.png
Testing/Data/Baseline/Filtering/ResampleImageFilter7PixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter8Test.1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter8Test.2.png
Testing/Data/Baseline/Filtering/ResampleImageFilter8Test.png
Testing/Data/Baseline/Filtering/ResampleImageFilter8TestPixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9Test.1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9Test.2.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9Test.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9TestPixelCentered.1.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9TestPixelCentered.2.png
Testing/Data/Baseline/Filtering/ResampleImageFilter9TestPixelCentered.png
Testing/Data/Baseline/Filtering/ResampleImageFilterTest1.png
Testing/Data/Baseline/Filtering/ResampleImageFilterTest2.png
Testing/Data/Baseline/Filtering/ResampleImageFilterTest3.png
Testing/Data/Baseline/Filtering/ResampleImageFilterTest4.png
Testing/Data/Baseline/Filtering/ResampleOrientedImageFilterTest.png
Testing/Data/Baseline/Filtering/ResampleVolumesToBeIsotropic.mha
Testing/Data/Baseline/Filtering/ScaleSpaceGenerator2D005.mhd
Testing/Data/Baseline/Filtering/ScaleSpaceGenerator2D005.raw
Testing/Data/Baseline/Filtering/SigmoidImageFilter.1.png
Testing/Data/Baseline/Filtering/SigmoidImageFilter.png
Testing/Data/Baseline/Filtering/SmoothingRecursiveGaussianImageFilter.png
Testing/Data/Baseline/Filtering/SmoothingRecursiveGaussianImageFilter2.png
Testing/Data/Baseline/Filtering/SubsampleVolume.mha
Testing/Data/Baseline/Filtering/ThresholdAboveImageFilter.png
Testing/Data/Baseline/Filtering/ThresholdBelowImageFilter.png
Testing/Data/Baseline/Filtering/ThresholdOutsideImageFilter.png
Testing/Data/Baseline/Filtering/VectorCurvatureAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/VectorGradientAnisotropicDiffusionImageFilter.png
Testing/Data/Baseline/Filtering/VectorIndexSelection.png
Testing/Data/Baseline/Filtering/VotingBinaryHoleFillingImageFilter1.png
Testing/Data/Baseline/Filtering/VotingBinaryHoleFillingImageFilter2.png
Testing/Data/Baseline/Filtering/VotingBinaryHoleFillingImageFilter3.png
Testing/Data/Baseline/Filtering/VotingBinaryIterativeHoleFillingImageFilter1.png
Testing/Data/Baseline/Filtering/VotingBinaryIterativeHoleFillingImageFilter2.png
Testing/Data/Baseline/Filtering/VotingBinaryIterativeHoleFillingImageFilter3.png
Testing/Data/Baseline/Filtering/ZeroCrossingBasedEdgeDetectionImageFilter1.png
Testing/Data/Baseline/Filtering/torus.nii.gz
Testing/Data/Baseline/Filtering/torus_multipleSeeds.nii.gz
Testing/Data/Baseline/Filtering/torus_multipleSeeds_NoHandlesTopo_out.nii.gz
Testing/Data/Baseline/Filtering/torus_multipleSeeds_NoTopo_out.nii.gz
Testing/Data/Baseline/Filtering/torus_multipleSeeds_StrictTopo_out.nii.gz
Testing/Data/Baseline/Filtering/torus_out.nii.gz
Testing/Data/Baseline/Filtering/wm.nii.gz
Testing/Data/Baseline/Filtering/wm_multipleSeeds.nii.gz
Testing/Data/Baseline/Filtering/wm_multipleSeeds_NoHandlesTopo_out.nii.gz
Testing/Data/Baseline/Filtering/wm_multipleSeeds_NoTopo_out.nii.gz
Testing/Data/Baseline/Filtering/wm_multipleSeeds_StrictTopo_out.nii.gz
Testing/Data/Baseline/Filtering/wm_out.nii.gz
Testing/Data/Baseline/IO/113766.003.001.mha
Testing/Data/Baseline/IO/19771.002.001.mha
Testing/Data/Baseline/IO/3868-2-100.mha
Testing/Data/Baseline/IO/48BitTestImage.tif
Testing/Data/Baseline/IO/HeadMRVolume.mhd
Testing/Data/Baseline/IO/HeadMRVolume.raw
Testing/Data/Baseline/IO/I.001.mha
Testing/Data/Baseline/IO/NrrdImageReadWriteTest1.nrrd
Testing/Data/Baseline/IO/NrrdRGBAImageReadWriteTest.png
Testing/Data/Baseline/IO/NrrdRGBImageReadWriteTest0.png
Testing/Data/Baseline/IO/NrrdRGBImageReadWriteTest1.png
Testing/Data/Baseline/IO/NrrdRGBImageReadWriteTest2.png
Testing/Data/Baseline/IO/PNGRGBAImageIOTestBaseline.png
Testing/Data/Baseline/IO/RGBTestImage.tif
Testing/Data/Baseline/IO/RGBTestImageCCITTFax3.mha
Testing/Data/Baseline/IO/RGBTestImageCCITTFax3.tif
Testing/Data/Baseline/IO/RGBTestImageCCITTFax4.mha
Testing/Data/Baseline/IO/RGBTestImageCCITTFax4.tif
Testing/Data/Baseline/IO/RGBTestImageGray.tif
Testing/Data/Baseline/IO/RGBTestImageHuffmanRLE.tif
Testing/Data/Baseline/IO/RGBTestImageJPEG.tif
Testing/Data/Baseline/IO/RGBTestImageLZW.tif
Testing/Data/Baseline/IO/RGBTestImageNone.tif
Testing/Data/Baseline/IO/RGBTestImagePackBits.tif
Testing/Data/Baseline/IO/RGBTestImageZIP.tif
Testing/Data/Baseline/IO/ShortTestImage.mha
Testing/Data/Baseline/IO/ShortTestImage.tiff
Testing/Data/Baseline/IO/biorad.mhd
Testing/Data/Baseline/IO/biorad.raw
Testing/Data/Baseline/IO/cthead1.bmp
Testing/Data/Baseline/IO/cthead1.jpg
Testing/Data/Baseline/IO/cthead1.png
Testing/Data/Baseline/IO/cthead1.tif
Testing/Data/Baseline/IO/cthead254x254.bmp
Testing/Data/Baseline/IO/image_color.bmp
Testing/Data/Baseline/IO/image_grayscale.bmp
Testing/Data/Baseline/IO/itkDICOMImageIO2Test.png
Testing/Data/Baseline/IO/itkDicomImageIOTest.png
Testing/Data/Baseline/IO/mini-complex-fast.nrrd
Testing/Data/Baseline/IO/mini-covector.nrrd
Testing/Data/Baseline/IO/mini-ten.nrrd
Testing/Data/Baseline/IO/mini-vector.nrrd
Testing/Data/Baseline/IO/ramp.mhd
Testing/Data/Baseline/IO/ramp.raw
Testing/Data/Baseline/IO/ramp.tif
Testing/Data/Baseline/IO/rampShort.tif
Testing/Data/Baseline/IO/small-tensors-double.nrrd
Testing/Data/Baseline/IO/small-tensors.nhdr
Testing/Data/Baseline/IO/small-tensors.raw
Testing/Data/Baseline/IO/smallRGBA.mha
Testing/Data/Baseline/IO/vol-ascii.nrrd
Testing/Data/Baseline/Iterators/ImageLinearIteratorWithIndexTest.png
Testing/Data/Baseline/Iterators/ImageRegionIteratorTest.png
Testing/Data/Baseline/Iterators/ImageRegionIteratorWithIndexTest.png
Testing/Data/Baseline/Iterators/ImageSliceIteratorWithIndexTest.mha
Testing/Data/Baseline/Patented/FuzzyConnectednessVWEyeTest.png
Testing/Data/Baseline/Registration/BSplineWarping1Test.png
Testing/Data/Baseline/Registration/DeformableRegistration12Test.png
Testing/Data/Baseline/Registration/DeformableRegistration13Test.png
Testing/Data/Baseline/Registration/DeformableRegistration2Test.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.1.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.2.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.3.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.4.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.5.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.6.png
Testing/Data/Baseline/Registration/DeformableRegistration3Test.png
Testing/Data/Baseline/Registration/DeformableRegistration4Test.png
Testing/Data/Baseline/Registration/DeformableRegistration4TestPixelCentered.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.1.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.2.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.3.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.4.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.5.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.6.png
Testing/Data/Baseline/Registration/DeformableRegistration5Test.png
Testing/Data/Baseline/Registration/DeformableRegistration6Test.png
Testing/Data/Baseline/Registration/DeformableRegistration6TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration10Test.png
Testing/Data/Baseline/Registration/ImageRegistration10TestPixelCentered.1.png
Testing/Data/Baseline/Registration/ImageRegistration10TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration12Test.png
Testing/Data/Baseline/Registration/ImageRegistration12TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration13Test.1.png
Testing/Data/Baseline/Registration/ImageRegistration13Test.2.png
Testing/Data/Baseline/Registration/ImageRegistration13Test.3.png
Testing/Data/Baseline/Registration/ImageRegistration13Test.4.png
Testing/Data/Baseline/Registration/ImageRegistration13Test.png
Testing/Data/Baseline/Registration/ImageRegistration13TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration14Test.1.png
Testing/Data/Baseline/Registration/ImageRegistration14Test.2.png
Testing/Data/Baseline/Registration/ImageRegistration14Test.png
Testing/Data/Baseline/Registration/ImageRegistration15Test.png
Testing/Data/Baseline/Registration/ImageRegistration15TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration16Test.png
Testing/Data/Baseline/Registration/ImageRegistration16TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration17Test.1.png
Testing/Data/Baseline/Registration/ImageRegistration17Test.png
Testing/Data/Baseline/Registration/ImageRegistration17TestPixelCentered.1.png
Testing/Data/Baseline/Registration/ImageRegistration17TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration1Test.1.png
Testing/Data/Baseline/Registration/ImageRegistration1Test.png
Testing/Data/Baseline/Registration/ImageRegistration1TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration2Test.png
Testing/Data/Baseline/Registration/ImageRegistration3Test.png
Testing/Data/Baseline/Registration/ImageRegistration3TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration4Test.png
Testing/Data/Baseline/Registration/ImageRegistration5Test1.png
Testing/Data/Baseline/Registration/ImageRegistration5Test1PixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration5Test2.png
Testing/Data/Baseline/Registration/ImageRegistration5Test2PixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration5oTest1PixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration6Test.png
Testing/Data/Baseline/Registration/ImageRegistration6TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration7Test.png
Testing/Data/Baseline/Registration/ImageRegistration7TestPixelCentered.png
Testing/Data/Baseline/Registration/ImageRegistration8RegisteredSlice.png
Testing/Data/Baseline/Registration/ImageRegistration9Test.png
Testing/Data/Baseline/Registration/ImageRegistration9TestPixelCentered.png
Testing/Data/Baseline/Registration/LandmarkWarping2Test.png
Testing/Data/Baseline/Registration/MultiResImageRegistration1Test.1.png
Testing/Data/Baseline/Registration/MultiResImageRegistration1Test.2.png
Testing/Data/Baseline/Registration/MultiResImageRegistration1Test.png
Testing/Data/Baseline/Registration/MultiResImageRegistration2Test.1.png
Testing/Data/Baseline/Registration/MultiResImageRegistration2Test.2.png
Testing/Data/Baseline/Registration/MultiResImageRegistration2Test.png
Testing/Data/Baseline/Review/GaborImageSourceTest0.mha
Testing/Data/Baseline/Review/GaborImageSourceTest1.mha
Testing/Data/Baseline/Review/N4ControlPoints_2D.nii.gz
Testing/Data/Baseline/Review/N4ControlPoints_3D.nii.gz
Testing/Data/Baseline/Review/RGBColormapTest_autumn.png
Testing/Data/Baseline/Review/RGBColormapTest_blue.png
Testing/Data/Baseline/Review/RGBColormapTest_cool.png
Testing/Data/Baseline/Review/RGBColormapTest_copper.png
Testing/Data/Baseline/Review/RGBColormapTest_green.png
Testing/Data/Baseline/Review/RGBColormapTest_hot.png
Testing/Data/Baseline/Review/RGBColormapTest_hsv.png
Testing/Data/Baseline/Review/RGBColormapTest_jet.png
Testing/Data/Baseline/Review/RGBColormapTest_overunder.png
Testing/Data/Baseline/Review/RGBColormapTest_red.png
Testing/Data/Baseline/Review/RGBColormapTest_spring.png
Testing/Data/Baseline/Review/RGBColormapTest_summer.png
Testing/Data/Baseline/Review/RGBColormapTest_winter.png
Testing/Data/Baseline/Review/Spots-binaryimage-to-shapelabel.mha
Testing/Data/Baseline/Review/Spots-binaryimage-to-statisticslabel.mha
Testing/Data/Baseline/Review/connected0lines-0.png
Testing/Data/Baseline/Review/connected0lines-1.png
Testing/Data/Baseline/Review/cthead1-0_0Overlay.png
Testing/Data/Baseline/Review/cthead1-0_1Overlay.png
Testing/Data/Baseline/Review/cthead1-0_2Overlay.png
Testing/Data/Baseline/Review/cthead1-0_3Overlay.png
Testing/Data/Baseline/Review/cthead1-0_4Overlay.png
Testing/Data/Baseline/Review/cthead1-0_5Overlay.png
Testing/Data/Baseline/Review/cthead1-0_6Overlay.png
Testing/Data/Baseline/Review/cthead1-0_7Overlay.png
Testing/Data/Baseline/Review/cthead1-0_8Overlay.png
Testing/Data/Baseline/Review/cthead1-0_9Overlay.png
Testing/Data/Baseline/Review/cthead1-1_0Overlay.png
Testing/Data/Baseline/Review/cthead1-binary-shape-opening.mha
Testing/Data/Baseline/Review/cthead1-keep-n-objects.mha
Testing/Data/Baseline/Review/cthead1-label-autocrop.mha
Testing/Data/Baseline/Review/cthead1-label-binary.mha
Testing/Data/Baseline/Review/cthead1-label-changed.mha
Testing/Data/Baseline/Review/cthead1-label-changed2.mha
Testing/Data/Baseline/Review/cthead1-label-changed3.png
Testing/Data/Baseline/Review/cthead1-label-changeregion.mha
Testing/Data/Baseline/Review/cthead1-label-crop.mha
Testing/Data/Baseline/Review/cthead1-label-pad.mha
Testing/Data/Baseline/Review/cthead1-label-regionreference.mha
Testing/Data/Baseline/Review/cthead1-label-relabeled.mha
Testing/Data/Baseline/Review/cthead1-label-shiftscaled.mha
Testing/Data/Baseline/Review/cthead1-labelAggregate.mha
Testing/Data/Baseline/Review/cthead1-labeled.mha
Testing/Data/Baseline/Review/cthead1-shape-opening.mha
Testing/Data/Baseline/Review/cthead1-shape-relabel-labelmap.mha
Testing/Data/Baseline/Review/cthead1Label-color.png
Testing/Data/Baseline/Review/cthead1RegionalMaximal-ref2_1.png
Testing/Data/Baseline/Review/cthead1RegionalMaximal-ref2_2.png
Testing/Data/Baseline/Review/cthead1RegionalMinimal-ref2_1.png
Testing/Data/Baseline/Review/cthead1RegionalMinimal-ref2_2.png
Testing/Data/Baseline/Review/cthead1RegionalMinimal-ref_2.png
Testing/Data/Baseline/Review/cthead1ValuedRegionalMaximal-ref.png
Testing/Data/Baseline/Review/cthead1ValuedRegionalMaximal-ref2.png
Testing/Data/Baseline/Review/cthead1ValuedRegionalMinimal-ref.png
Testing/Data/Baseline/Review/cthead1ValuedRegionalMinimal-ref2.png
Testing/Data/Baseline/Review/itkAreaClosingImageFilterTest0.png
Testing/Data/Baseline/Review/itkAreaOpeningImageFilterTest0.png
Testing/Data/Baseline/Review/itkAttributeKeepNObjectsLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkAttributeKeepNObjectsLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkAttributeOpeningLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkAttributeOpeningLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkAttributePositionLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkAttributeRelabelLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkAttributeRelabelLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkAttributeUniqueLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkAttributeUniqueLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkAutoCropLabelMapFilterTest2-163.png
Testing/Data/Baseline/Review/itkAutoCropLabelMapFilterTest2-92.png
Testing/Data/Baseline/Review/itkBSplineScatteredDataPointSetToImageFilterTest01.mha
Testing/Data/Baseline/Review/itkBinaryClosingByReconstructionImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryContourImageFilterTest0.png
Testing/Data/Baseline/Review/itkBinaryContourImageFilterTest1.png
Testing/Data/Baseline/Review/itkBinaryFillholeImageFilterTest1.png
Testing/Data/Baseline/Review/itkBinaryGrindPeakImageFilterTest1.png
Testing/Data/Baseline/Review/itkBinaryMorphologicalClosingImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryMorphologicalClosingImageFilterTestUnsafe.png
Testing/Data/Baseline/Review/itkBinaryMorphologicalOpeningImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryOpeningByReconstructionImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryReconstructionByDilationImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryReconstructionByErosionImageFilterTest.png
Testing/Data/Baseline/Review/itkBinaryReconstructionLabelMapFilterTest.png
Testing/Data/Baseline/Review/itkBoxMeanImageFilter10.png
Testing/Data/Baseline/Review/itkBoxMeanImageFilter3.png
Testing/Data/Baseline/Review/itkBoxSigmaImageFilter10.png
Testing/Data/Baseline/Review/itkBoxSigmaImageFilter3.png
Testing/Data/Baseline/Review/itkConvolutionImageFilterTest5x5Mean.png
Testing/Data/Baseline/Review/itkConvolutionImageFilterTestSobelX.nii.gz
Testing/Data/Baseline/Review/itkConvolutionImageFilterTestSobelY.nii.gz
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest01.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest02.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest03.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest10.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest11.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest12.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest20.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest21.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFilterTest30.png
Testing/Data/Baseline/Review/itkDiscreteGaussianDerivativeImageFunctionTest01.png
Testing/Data/Baseline/Review/itkDiscreteGradientMagnitudeGaussianImageFunctionTest01.png
Testing/Data/Baseline/Review/itkDiscreteHessianGaussianImageFunctionTest01_0.mha
Testing/Data/Baseline/Review/itkDiscreteHessianGaussianImageFunctionTest01_1.mha
Testing/Data/Baseline/Review/itkDiscreteHessianGaussianImageFunctionTest01_2.mha
Testing/Data/Baseline/Review/itkFFTShiftImageFilterTest0.png
Testing/Data/Baseline/Review/itkFFTShiftImageFilterTestEven0.png
Testing/Data/Baseline/Review/itkFastApproximateRankImageFilter10.png
Testing/Data/Baseline/Review/itkFastApproximateRankImageFilter3.png
Testing/Data/Baseline/Review/itkGridImageSourceTest1.nhdr
Testing/Data/Baseline/Review/itkGridImageSourceTest1.raw.gz
Testing/Data/Baseline/Review/itkGridImageSourceTest2.nhdr
Testing/Data/Baseline/Review/itkGridImageSourceTest2.raw.gz
Testing/Data/Baseline/Review/itkGridImageSourceTest3.nhdr
Testing/Data/Baseline/Review/itkGridImageSourceTest3.raw.gz
Testing/Data/Baseline/Review/itkGridImageSourceTest4.nhdr
Testing/Data/Baseline/Review/itkGridImageSourceTest4.raw.gz
Testing/Data/Baseline/Review/itkHessianToObjectnessMeasureImageFilterTest.mha
Testing/Data/Baseline/Review/itkHessianToObjectnessMeasureImageFilterTest2.mha
Testing/Data/Baseline/Review/itkLabelContourImageFilterTest0.png
Testing/Data/Baseline/Review/itkLabelContourImageFilterTest1.png
Testing/Data/Baseline/Review/itkLabelGeometryImageFilterTest2Image.png
Testing/Data/Baseline/Review/itkLabelMapContourOverlayImageFilterTest0.png
Testing/Data/Baseline/Review/itkLabelMapContourOverlayImageFilterTest1.png
Testing/Data/Baseline/Review/itkLabelMapContourOverlayImageFilterTest2.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-0-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-0-0-1.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-0-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-0-1-1.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-100-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-100-0-1.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-100-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTest-100-1-1.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-0-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-0-0-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-0-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-0-1-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-1-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-1-0-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-1-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-0-1-1-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-0-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-0-0-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-0-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-0-1-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-1-0-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-1-0-10.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-1-1-0.png
Testing/Data/Baseline/Review/itkLabelMapMaskImageFilterTestCrop-100-1-1-10.png
Testing/Data/Baseline/Review/itkLabelMapOverlayImageFilterTest1.png
Testing/Data/Baseline/Review/itkLabelMapToAttributeImageFilterTest1.png
Testing/Data/Baseline/Review/itkLabelMapToRGBImageFilterTest1.png
Testing/Data/Baseline/Review/itkLabelSelectionLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkLabelSelectionLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkLabelUniqueLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkLabelUniqueLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkMaskedRankImageFilter10.png
Testing/Data/Baseline/Review/itkMaskedRankImageFilter3.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedFromMarkersImageFilterTestM0F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedFromMarkersImageFilterTestM0F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedFromMarkersImageFilterTestM1F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedFromMarkersImageFilterTestM1F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestButtonHoleM0F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestButtonHoleM0F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestButtonHoleM1F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestButtonHoleM1F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel00.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel10.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel20.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel30.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel40.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestLevel50.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPassValuesM0F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPassValuesM0F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPassValuesM1F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPassValuesM1F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPlateauM0F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPlateauM0F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPlateauM1F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestPlateauM1F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestThickLinesM0F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestThickLinesM0F1.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestThickLinesM1F0.png
Testing/Data/Baseline/Review/itkMorphologicalWatershedImageFilterTestThickLinesM1F1.png
Testing/Data/Baseline/Review/itkObjectByObjectLabelMapFilterTest0.png
Testing/Data/Baseline/Review/itkObjectByObjectLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkRankImageFilter10.png
Testing/Data/Baseline/Review/itkRankImageFilter3.png
Testing/Data/Baseline/Review/itkRobustAutomaticThresholdImageFilterTest.png
Testing/Data/Baseline/Review/itkScalarChanAndVeseDenseLevelSetImageFilterTest2_0.mha
Testing/Data/Baseline/Review/itkScalarChanAndVeseDenseLevelSetImageFilterTest2_1.mha
Testing/Data/Baseline/Review/itkScalarChanAndVeseDenseLevelSetImageFilterTest2_2.mha
Testing/Data/Baseline/Review/itkScalarChanAndVeseSparseLevelSetImageFilterTest2.mha
Testing/Data/Baseline/Review/itkShapePositionLabelMapFilterTest1.png
Testing/Data/Baseline/Review/itkStochasticFractalDimensionImageFilterTest1.mha
Testing/Data/Baseline/Review/itkStochasticFractalDimensionImageFilterTest2.mha
Testing/Data/Baseline/Review/nonConnected3DLines-0.tif
Testing/Data/Baseline/Review/reconstruct.mha
Testing/Data/Baseline/Review/simple-label-merge-aggregate.mha
Testing/Data/Baseline/Review/simple-label-merge-keep.mha
Testing/Data/Baseline/Review/simple-label-merge-pack.mha
Testing/Data/Baseline/Review/simple-label-merge-strict.mha
Testing/Data/Baseline/Review/simple-label-to-shapelabelmap.mha
Testing/Data/Baseline/Segmentation/CannySegmentationLevelSetImageFilterTest.png
Testing/Data/Baseline/Segmentation/ConfidenceConnectedGrayTest.png
Testing/Data/Baseline/Segmentation/ConfidenceConnectedVentricleTest.png
Testing/Data/Baseline/Segmentation/ConfidenceConnectedWhiteTest.png
Testing/Data/Baseline/Segmentation/ConnectedThresholdGrayTest.png
Testing/Data/Baseline/Segmentation/ConnectedThresholdVentricleTest.png
Testing/Data/Baseline/Segmentation/ConnectedThresholdWhiteTest.png
Testing/Data/Baseline/Segmentation/CurvesLevelSetImageFilterTest.png
Testing/Data/Baseline/Segmentation/FastMarchingGrayMatterTest.1.png
Testing/Data/Baseline/Segmentation/FastMarchingGrayMatterTest.png
Testing/Data/Baseline/Segmentation/FastMarchingLeftVentricleTest.1.png
Testing/Data/Baseline/Segmentation/FastMarchingLeftVentricleTest.png
Testing/Data/Baseline/Segmentation/FastMarchingRightVentricleTest.1.png
Testing/Data/Baseline/Segmentation/FastMarchingRightVentricleTest.png
Testing/Data/Baseline/Segmentation/FastMarchingWhiteMatterTest.1.png
Testing/Data/Baseline/Segmentation/FastMarchingWhiteMatterTest.png
Testing/Data/Baseline/Segmentation/FuzzyConnectednessVWEyeTest.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourGrayMatterTest.1.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourGrayMatterTest.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourLeftVentricleTest.1.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourLeftVentricleTest.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourRightVentricleTest.1.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourRightVentricleTest.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourShapePriorCallosumTest.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourWhiteMatterTest.1.png
Testing/Data/Baseline/Segmentation/GeodesicActiveContourWhiteMatterTest.png
Testing/Data/Baseline/Segmentation/HoughTransformTwoCirclesTest.png
Testing/Data/Baseline/Segmentation/HoughTransformTwoLinesTest.png
Testing/Data/Baseline/Segmentation/IsolatedConnectedTest.png
Testing/Data/Baseline/Segmentation/LaplacianSegmentationLevelSetImageFilter.png
Testing/Data/Baseline/Segmentation/NeighborhoodConnectedGrayTest.png
Testing/Data/Baseline/Segmentation/NeighborhoodConnectedVentricleTest.png
Testing/Data/Baseline/Segmentation/NeighborhoodConnectedWhiteTest.png
Testing/Data/Baseline/Segmentation/RelabelComponentImageFilter.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetGrayMatterTest.1.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetGrayMatterTest.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetLeftVentricleTest.1.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetLeftVentricleTest.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetRightVentricleTest.1.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetRightVentricleTest.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetWhiteMatterTest.1.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetWhiteMatterTest.2.png
Testing/Data/Baseline/Segmentation/ShapeDetectionLevelSetWhiteMatterTest.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetGrayMatterTest.1.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetGrayMatterTest.2.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetGrayMatterTest.3.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetGrayMatterTest.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetVentricle.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetVentricleTest.1.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetVentricleTest.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetWhiteMatterTest.1.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetWhiteMatterTest.2.png
Testing/Data/Baseline/Segmentation/ThresholdSegmentationLevelSetWhiteMatterTest.png
Testing/Data/Baseline/Segmentation/VectorConfidenceConnected1.png
Testing/Data/Baseline/Segmentation/VectorConfidenceConnected2.png
Testing/Data/Baseline/Segmentation/VectorConfidenceConnected3.png
Testing/Data/Baseline/Segmentation/WatershedSegmentation1Test1.png
Testing/Data/Baseline/Segmentation/WatershedSegmentation1Test2.png
Testing/Data/Baseline/SpatialObjects/BoxSpatialObjectTest.png
Testing/Data/Baseline/Statistics/BayesianClassifierInitializerClass2Output.png
Testing/Data/Baseline/Statistics/BayesianClassifierLabelMap.png
Testing/Data/Baseline/Statistics/ImageClassifierFilterTestClassifiedImageTest.png
Testing/Data/Baseline/Statistics/ScalarImageKmeansClassifierOutput.png
Testing/Data/Baseline/Statistics/ScalarImageMarkovRandomField1Output.png
Testing/Data/Baseline/Statistics/itkSampleSelectiveMeanShiftBlurringFilterTest.png
Testing/Data/Baseline/Statistics/itkSampleSelectiveMeanShiftBlurringFilterTestPixelCentered.png
Testing/Data/ITKData.readme
Testing/Data/Input/012345.002.050
Testing/Data/Input/012345.002.050.README
Testing/Data/Input/289.png
Testing/Data/Input/2th_cthead1.png
Testing/Data/Input/48BitTestImage.tif
Testing/Data/Input/5x5-constant.png
Testing/Data/Input/8ConnectedImage.bmp
Testing/Data/Input/Archetype/a5b1.PNG
Testing/Data/Input/Archetype/a5b6.PNG
Testing/Data/Input/Archetype/a5b7.PNG
Testing/Data/Input/Archetype/a6b1.PNG
Testing/Data/Input/Archetype/a7b1.PNG
Testing/Data/Input/Archetype/a7b2.PNG
Testing/Data/Input/Archetype/a7b3.PNG
Testing/Data/Input/Archetype/a7b4.PNG
Testing/Data/Input/Archetype/a8b1.PNG
Testing/Data/Input/Archetype/image.001
Testing/Data/Input/Archetype/image.002
Testing/Data/Input/Archetype/image.003
Testing/Data/Input/Archetype/image.004
Testing/Data/Input/Archetype/image.005
Testing/Data/Input/Archetype/image.006
Testing/Data/Input/Archetype/image.007
Testing/Data/Input/Archetype/image.008
Testing/Data/Input/Archetype/image.009
Testing/Data/Input/Archetype/image.010
Testing/Data/Input/BSplineScatteredApproximationDataPointsInput.txt
Testing/Data/Input/BigEndian.hdr
Testing/Data/Input/BigEndian.img
Testing/Data/Input/BigEndian.mhd
Testing/Data/Input/BigEndian.spr
Testing/Data/Input/BigEndianCompressed.hdr
Testing/Data/Input/BigEndianCompressed.img
Testing/Data/Input/BigEndianCompressed.img.gz
Testing/Data/Input/BinaryImageWithVariousShapes01.png
Testing/Data/Input/BrainSliceBinary.png
Testing/Data/Input/Bretagne1.j2k
Testing/Data/Input/CellsFluorescence1.png
Testing/Data/Input/CellsFluorescence2.png
Testing/Data/Input/Cevennes2.jp2
Testing/Data/Input/Colormap_Grey.png
Testing/Data/Input/ContourExtractor2DTest.tif
Testing/Data/Input/DCB120604.t61/11/acqp
Testing/Data/Input/DCB120604.t61/11/imnd
Testing/Data/Input/DCB120604.t61/11/pdata/1/2dseq
Testing/Data/Input/DCB120604.t61/11/pdata/1/d3proc
Testing/Data/Input/DCB120604.t61/11/pdata/1/meta
Testing/Data/Input/DCB120604.t61/11/pdata/1/procs
Testing/Data/Input/DCB120604.t61/11/pdata/1/reco
Testing/Data/Input/DCB120604.t61/11/pdata/1/roi
Testing/Data/Input/DCB120604.t61/11/pulseprogram
Testing/Data/Input/DCB120604.t61/11/spnam0
Testing/Data/Input/DCB120604.t61/11/spnam1
Testing/Data/Input/DCB120604.t61/AdjStatePerStudy
Testing/Data/Input/DCB120604.t61/subject
Testing/Data/Input/DSA.png
Testing/Data/Input/DicomSeries/Image0075.dcm
Testing/Data/Input/DicomSeries/Image0076.dcm
Testing/Data/Input/DicomSeries/Image0077.dcm
Testing/Data/Input/DwiCorpusCallosum.nhdr
Testing/Data/Input/DwiCorpusCallosum.raw
Testing/Data/Input/FEM/3dscope-big.fem
Testing/Data/Input/FEM/3dscope.fem
Testing/Data/Input/FEM/cell10.fem
Testing/Data/Input/FEM/cell2.fem
Testing/Data/Input/FEM/cell3.fem
Testing/Data/Input/FEM/cell4.fem
Testing/Data/Input/FEM/cell5.fem
Testing/Data/Input/FEM/hexa2.fem
Testing/Data/Input/FEM/hexa3.fem
Testing/Data/Input/FEM/hexa4-grav.fem
Testing/Data/Input/FEM/input-list
Testing/Data/Input/FEM/quad-lm.fem
Testing/Data/Input/FEM/quad2-small.fem
Testing/Data/Input/FEM/quad2-strain.fem
Testing/Data/Input/FEM/quad3-nodal.fem
Testing/Data/Input/FEM/quad3.fem
Testing/Data/Input/FEM/quad4.fem
Testing/Data/Input/FEM/quad5.fem
Testing/Data/Input/FEM/quad6-grav.fem
Testing/Data/Input/FEM/tetra2.fem
Testing/Data/Input/FEM/tetra3.fem
Testing/Data/Input/FEM/tetra4-grav.fem
Testing/Data/Input/FEM/trapezoid.fem
Testing/Data/Input/FEM/tri2.fem
Testing/Data/Input/FEM/tri3-e.fem
Testing/Data/Input/FEM/tri3-q.fem
Testing/Data/Input/FEM/tri3.fem
Testing/Data/Input/FEM/truss.fem
Testing/Data/Input/FEM/truss_noload.fem
Testing/Data/Input/GradientRecursiveGaussianImageFilterTest.mha
Testing/Data/Input/GrayscaleImageWithVariousShapes01.png
Testing/Data/Input/HeadMRVolume.cub
Testing/Data/Input/HeadMRVolume.mha
Testing/Data/Input/HeadMRVolume.mhd
Testing/Data/Input/HeadMRVolume.raw
Testing/Data/Input/HeadMRVolume1.cub
Testing/Data/Input/HeadMRVolume1.cub.gz
Testing/Data/Input/HeadMRVolume2.cub
Testing/Data/Input/HeadMRVolume2.cub.gz
Testing/Data/Input/HeadMRVolumeCompressed.mha
Testing/Data/Input/HeadMRVolumeWithDirection.mhd
Testing/Data/Input/HeadMRVolumeWithDirection.nhdr
Testing/Data/Input/HeadMRVolumeWithDirection.raw.gz
Testing/Data/Input/HeadMRVolumeWithDirection001.mhd
Testing/Data/Input/HeadMRVolumeWithDirection002.mhd
Testing/Data/Input/HeadMRVolumeWithDirection003.mhd
Testing/Data/Input/HeadMRVolumeWithDirectionIdentity.mhd
Testing/Data/Input/HeadMRVolumeWithDirectionIdentity.nhdr
Testing/Data/Input/HeadMRVolumeWithDirectionIdentity.raw.gz
Testing/Data/Input/IntensityRamp64.png
Testing/Data/Input/LittleEndian.hdr
Testing/Data/Input/LittleEndian.img
Testing/Data/Input/LittleEndian.mhd
Testing/Data/Input/LittleEndianCompressed.hdr
Testing/Data/Input/LittleEndianCompressed.img.gz
Testing/Data/Input/LittleEndianZ.hdr
Testing/Data/Input/LittleEndianZ.img.gz
Testing/Data/Input/LungSliceBinary.png
Testing/Data/Input/MetaImageError.mhd
Testing/Data/Input/NeuralNetworks/qpropxortest.txt
Testing/Data/Input/NeuralNetworks/rbf_multivar_gauss_test.txt
Testing/Data/Input/NeuralNetworks/rbf_multivar_gauss_train.txt
Testing/Data/Input/NeuralNetworks/test.txt
Testing/Data/Input/NeuralNetworks/train.txt
Testing/Data/Input/NeuralNetworks/xortest.txt
Testing/Data/Input/Number1inText.png
Testing/Data/Input/Number2inText.png
Testing/Data/Input/Number3inText.png
Testing/Data/Input/PNGRGBAImageIOTestInput.png
Testing/Data/Input/RGBTestImage.tif
Testing/Data/Input/RGBTestImageCCITTFax3.tif
Testing/Data/Input/RGBTestImageCCITTFax4.tif
Testing/Data/Input/RGBTestImageGray.tif
Testing/Data/Input/RGBTestImageHuffmanRLE.tif
Testing/Data/Input/RGBTestImageJPEG.tif
Testing/Data/Input/RGBTestImageLZW.tif
Testing/Data/Input/RGBTestImageNone.tif
Testing/Data/Input/RGBTestImagePackBits.tif
Testing/Data/Input/RGBTestImageZIP.tif
Testing/Data/Input/STAPLE1.png
Testing/Data/Input/STAPLE2.png
Testing/Data/Input/STAPLE3.png
Testing/Data/Input/STAPLE4.png
Testing/Data/Input/Shapes.png
Testing/Data/Input/ShortTestImage.tiff
Testing/Data/Input/SpatialObjects.meta
Testing/Data/Input/Spots-markers.png
Testing/Data/Input/Spots.png
Testing/Data/Input/SpotsInverted.png
Testing/Data/Input/SpotsLabeled.png
Testing/Data/Input/SquareBinary201.png
Testing/Data/Input/Statistics/BrainT1Slice_AfterKMeansClassification.png
Testing/Data/Input/Statistics/TwoDimensionTwoGaussian.Readme
Testing/Data/Input/Statistics/TwoDimensionTwoGaussian.dat
Testing/Data/Input/Statistics/TwoDimensionTwoGaussianTesting.dat
Testing/Data/Input/Statistics/pointsForKdTree.txt
Testing/Data/Input/Statistics/pointsForKdTree2.txt
Testing/Data/Input/Statistics/pointsForQuickSelect1.txt
Testing/Data/Input/Statistics/pointsForQuickSelect2.txt
Testing/Data/Input/Statistics/pointsForQuickSelect3.txt
Testing/Data/Input/T1_MEASUREMENT_4_1.PAR
Testing/Data/Input/T1_MEASUREMENT_4_1.REC
Testing/Data/Input/TensorsCorpusCallosum.mhd
Testing/Data/Input/TensorsCorpusCallosum.raw
Testing/Data/Input/TreeBarkTexture.png
Testing/Data/Input/VHFColor.mhd
Testing/Data/Input/VHFColor.raw
Testing/Data/Input/VisibleWomanEyeSlice.png
Testing/Data/Input/VisibleWomanEyeSliceInitialLevelSet.png
Testing/Data/Input/biorad.pic
Testing/Data/Input/box.nhdr
Testing/Data/Input/box.raw
Testing/Data/Input/button-hole.png
Testing/Data/Input/cake_easy.png
Testing/Data/Input/cake_hard.png
Testing/Data/Input/chondt.png
Testing/Data/Input/circle.png
Testing/Data/Input/circle100.png
Testing/Data/Input/closerec1.jpg
Testing/Data/Input/connected0lines.png
Testing/Data/Input/cthead1-bin.png
Testing/Data/Input/cthead1-markers.png
Testing/Data/Input/cthead1-mask.png
Testing/Data/Input/cthead1-multires.tif
Testing/Data/Input/cthead1.bmp
Testing/Data/Input/cthead1.j2k
Testing/Data/Input/cthead1.jpg
Testing/Data/Input/cthead1.lsm
Testing/Data/Input/cthead1.png
Testing/Data/Input/cthead1.tif
Testing/Data/Input/cthead1Label.png
Testing/Data/Input/cthead254x254.bmp
Testing/Data/Input/dicom-sc_cs-1.dcm
Testing/Data/Input/foot.mha
Testing/Data/Input/foot.raw
Testing/Data/Input/genusZeroSurface01.vtk
Testing/Data/Input/image_color.bmp
Testing/Data/Input/image_grayscale.bmp
Testing/Data/Input/itkAffineTransform00.txt
Testing/Data/Input/itkAffineTransform01.txt
Testing/Data/Input/itkBSplineTransform00.txt
Testing/Data/Input/itkDeformationFieldSourceTestLandmarks.txt
Testing/Data/Input/itkGDCMImageIOTest.dcm
Testing/Data/Input/itkGDCMImageIOTest3.dcm
Testing/Data/Input/level.png
Testing/Data/Input/metamesh.txt
Testing/Data/Input/mini-complex-slow.nrrd
Testing/Data/Input/mini-covector-fast.nrrd
Testing/Data/Input/mini-covector-slow.nrrd
Testing/Data/Input/mini-ten-mask.nrrd
Testing/Data/Input/mini-ten-nomask-fast.nrrd
Testing/Data/Input/mini-ten-nomask-slow.nrrd
Testing/Data/Input/mini-vector-fast.nrrd
Testing/Data/Input/mini-vector-slow.nrrd
Testing/Data/Input/model.mha
Testing/Data/Input/model.raw
Testing/Data/Input/mri3D.mhd
Testing/Data/Input/mushroom.vtk
Testing/Data/Input/nonConnected3DLines.tif
Testing/Data/Input/parametersBSpline.txt
Testing/Data/Input/pass-values.png
Testing/Data/Input/peppers.png
Testing/Data/Input/plateau.png
Testing/Data/Input/pointsForKdTree.txt
Testing/Data/Input/pointsForKdTree2.txt
Testing/Data/Input/pointsForQuickSelect1.txt
Testing/Data/Input/pointsForQuickSelect2.txt
Testing/Data/Input/pointsForQuickSelect3.txt
Testing/Data/Input/ramp.gipl
Testing/Data/Input/ramp.gipl.gz
Testing/Data/Input/ramp.tif
Testing/Data/Input/rampShort.tif
Testing/Data/Input/sf4.png
Testing/Data/Input/simple-label-a.png
Testing/Data/Input/simple-label-b.png
Testing/Data/Input/simple-label-c.png
Testing/Data/Input/simple-label-d.png
Testing/Data/Input/simple-label-e.png
Testing/Data/Input/sinogram-small.mha
Testing/Data/Input/small-tensors.nhdr
Testing/Data/Input/small-tensors.raw
Testing/Data/Input/smallRGBA.bmp
Testing/Data/Input/smooth_circle.png
Testing/Data/Input/smooth_square.png
Testing/Data/Input/sobel_x.nii.gz
Testing/Data/Input/sobel_y.nii.gz
Testing/Data/Input/sphereBinary.vtk
Testing/Data/Input/test_ge4/19771.002.001
Testing/Data/Input/test_ge4/19771.002.002
Testing/Data/Input/test_ge4/19771.002.003
Testing/Data/Input/test_ge4/19771.002.004
Testing/Data/Input/test_ge4/19771.002.005
Testing/Data/Input/test_ge4/19771.002.006
Testing/Data/Input/test_ge5/113766.003.001
Testing/Data/Input/test_ge5/113766.003.002
Testing/Data/Input/test_ge5/113766.003.003
Testing/Data/Input/test_ge5/113766.003.004
Testing/Data/Input/test_ge5/113766.003.005
Testing/Data/Input/test_ge5/113766.003.006
Testing/Data/Input/test_geadw/I.001
Testing/Data/Input/test_geadw/I.002
Testing/Data/Input/test_geadw/I.003
Testing/Data/Input/test_geadw/I.004
Testing/Data/Input/test_geadw/I.005
Testing/Data/Input/test_geadw/I.006
Testing/Data/Input/test_siemens/3868-2-100.ima
Testing/Data/Input/test_siemens/3868-2-101.ima
Testing/Data/Input/test_siemens/3868-2-102.ima
Testing/Data/Input/test_siemens/3868-2-103.ima
Testing/Data/Input/test_siemens/3868-2-104.ima
Testing/Data/Input/test_siemens/3868-2-105.ima
Testing/Data/Input/testrgb-0.nhdr
Testing/Data/Input/testrgb-0.raw
Testing/Data/Input/testrgb-1.nhdr
Testing/Data/Input/testrgb-1.raw
Testing/Data/Input/testrgb-2.nhdr
Testing/Data/Input/testrgb-2.raw
Testing/Data/Input/testrgba.nrrd
Testing/Data/Input/tetrahedron.vtk
Testing/Data/Input/thick-lines.png
Testing/Data/Input/vol-ascii.ascii
Testing/Data/Input/vol-ascii.nhdr
Testing/Data/Input/vol-ascii.nrrd
Testing/Data/Input/vol-gzip-big.nhdr
Testing/Data/Input/vol-gzip-big.nrrd
Testing/Data/Input/vol-gzip-big.raw.gz
Testing/Data/Input/vol-gzip-little.nhdr
Testing/Data/Input/vol-gzip-little.nrrd
Testing/Data/Input/vol-gzip-little.raw.gz
Testing/Data/Input/vol-raw-big.nhdr
Testing/Data/Input/vol-raw-big.nrrd
Testing/Data/Input/vol-raw-big.raw
Testing/Data/Input/vol-raw-little.nhdr
Testing/Data/Input/vol-raw-little.nrrd
Testing/Data/Input/vol-raw-little.raw
Testing/Data/Input/vtkPolyDataBad0.vtk
Testing/Data/Input/vtkPolyDataBad1.vtk
Testing/Data/Input/vtkPolyDataBad10.vtk
Testing/Data/Input/vtkPolyDataBad11.vtk
Testing/Data/Input/vtkPolyDataBad12.vtk
Testing/Data/Input/vtkPolyDataBad13.vtk
Testing/Data/Input/vtkPolyDataBad14.vtk
Testing/Data/Input/vtkPolyDataBad15.vtk
Testing/Data/Input/vtkPolyDataBad16.vtk
Testing/Data/Input/vtkPolyDataBad17.vtk
Testing/Data/Input/vtkPolyDataBad2.vtk
Testing/Data/Input/vtkPolyDataBad3.vtk
Testing/Data/Input/vtkPolyDataBad4.vtk
Testing/Data/Input/vtkPolyDataBad5.vtk
Testing/Data/Input/vtkPolyDataBad6.vtk
Testing/Data/Input/vtkPolyDataBad7.vtk
Testing/Data/Input/vtkPolyDataBad8.vtk
Testing/Data/Input/vtkPolyDataBad9.vtk
Testing/Data/Input/whiteSpot.png
Testing/Data/Input/xornet.txt
Testing/Data/Input/xortest.txt
)

foreach(CURR_DATA_FILE ${OLD_DATA_FILES})
  itk_add_test(NAME JUSTGETDATA_${CURR_DATA_FILE} COMMAND /bin/ls DATA{${CURR_DATA_FILE}})
endforeach()
