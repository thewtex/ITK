#
# This function will prevent in-source builds
function(AssureOutOfSourceBuilds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if("${srcdir}" STREQUAL "${bindir}")
    message(STATUS
      "######################################################")
    message(STATUS
      "# ITK should not be configured & built in the ITK source directory")
    message(STATUS
      "# You must run cmake in a build directory.")
    message(STATUS
      "# For example:")
    message(STATUS
      "# mkdir ITK-Sandbox ; cd ITK-sandbox")
    message(STATUS
      "# git clone git://itk.org/ITK.git # or download & unpack the source tarball")
    message(STATUS
      "# mkdir ITK-build ; cd ITK-build ; ccmake ../ITK")
    message(STATUS
      "# NOTE: CMake will have created several files & directories")
    message(STATUS
      "#       in your source tree. run 'git status' to find them and")
    message(STATUS
      "#       remove them.")
    message(STATUS
      "######################################################")
    message(FATAL_ERROR "Quitting configuration")
  endif()
endfunction()

AssureOutOfSourceBuilds()
