set(CTEST_SITE "$ENV{NODE_NAME}.azure")
set(CTEST_CMAKE_GENERATOR "Ninja")
if(UNIX)
  set(ENV{CC} gcc)
  set(ENV{CXX} g++)
  set(PLATFORM "Linux")
  set(COMPILER "GCC-4.8.2")
  set(CTEST_SOURCE_DIRECTORY "/jenkins/src/ITK")
  set(ExternalData_OBJECT_STORES "/jenkins/data")
elseif(WIN32)
  set(PLATFORM "Windows")
  set(COMPILER "VS12")
  set(CTEST_SOURCE_DIRECTORY "C:/Jenkins/src/ITK")
  set(ExternalData_OBJECT_STORES "C:/Jenkins/data")
else()
endif()
set(CTEST_BUILD_NAME "${PLATFORM}-${COMPILER}-$ENV{GERRIT_TOPIC}-$ENV{GERRIT_CHANGE_NUMBER}-$ENV{GERRIT_PATCHSET_NUMBER}")
set(CTEST_BUILD_CONFIGURATION Release)
set(CTEST_DASHBOARD_ROOT "$ENV{WORKSPACE}")
set(CTEST_BINARY_DIRECTORY "ITK-bin")
set(dashboard_model "Experimental")
set(dashboard_track "Gerrit")
set(dashboard_no_clean 1)

message("CTEST_SITE = ${CTEST_SITE}")
message("CTEST_BUILD_NAME = ${CTEST_BUILD_NAME}")
include(${CTEST_SCRIPT_DIRECTORY}/itk_common.cmake)
