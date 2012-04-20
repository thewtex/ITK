# Function to fetch remote modules.

# Helper function to fetch a module stored in a Git repository.
# Git fetches are only performed when required.
function(_fetch_with_git git_executable git_repository git_tag module_dir)
  if("${git_tag}" STREQUAL "" OR "${git_repository}" STREQUAL "")
    message(FATAL_ERROR "Tag or repository for git checkout should not be empty.")
  endif()

  # If we don't have a clone yet.
  if(NOT EXISTS "${module_dir}")
    message("git_EXECUTABLE: ${git_EXECUTABLE}
    git_repository: ${git_repository}
    module_dir: ${module_dir}
    "
    )
    execute_process(
      COMMAND "${git_executable}" clone "${git_repository}" "${module_dir}"
      RESULT_VARIABLE error_code
      )
    if(error_code)
      message(FATAL_ERROR "Failed to clone repository: '${git_repository}'")
    endif()

    execute_process(
      COMMAND "${git_executable}" checkout ${git_tag}
      WORKING_DIRECTORY "${module_dir}"
      RESULT_VARIABLE error_code
      )
    if(error_code)
      message(FATAL_ERROR "Failed to checkout tag: '${git_tag}'")
    endif()

    execute_process(
      COMMAND "${git_executable}" submodule init
      WORKING_DIRECTORY "${module_dir}"
      RESULT_VARIABLE error_code
      )
    if(error_code)
      message(FATAL_ERROR "Failed to init submodules in: '${module_dir}'")
    endif()

    execute_process(
      COMMAND "${git_executable}" submodule update --recursive
      WORKING_DIRECTORY "${module_dir}"
      RESULT_VARIABLE error_code
      )
    if(error_code)
      message(FATAL_ERROR "Failed to update submodules in: '${module_dir}'")
    endif()
  else() # We already have a clone, but we need to check that it has the right revision.
    execute_process(
      COMMAND "${git_executable}" rev-parse --verify ${git_tag}
      WORKING_DIRECTORY "${module_dir}"
      RESULT_VARIABLE error_code
      OUTPUT_VARIABLE tag_hash
      )
    if(error_code)
      message(FATAL_ERROR "Failed to get the hash for tag '${module_dir}'")
    endif()
    execute_process(
      COMMAND "${git_executable}" rev-parse --verify HEAD
      WORKING_DIRECTORY "${module_dir}"
      RESULT_VARIABLE error_code
      OUTPUT_VARIABLE head_hash
      )
    if(error_code)
      message(FATAL_ERROR "Failed to get the hash for ${git_repository} HEAD")
    endif()

    # Is the hash checkout out that we want?
    if(NOT ("${tag_hash}" STREQUAL "${head_hash}"))
      execute_process(
        COMMAND "${git_executable}" fetch
        WORKING_DIRECTORY "${module_dir}"
        RESULT_VARIABLE error_code
        )
      if(error_code)
        message(FATAL_ERROR "Failed to fetch repository '${git_repository}'")
      endif()

      execute_process(
        COMMAND "${git_executable}" checkout ${git_tag}
        WORKING_DIRECTORY "${module_dir}"
        RESULT_VARIABLE error_code
        )
      if(error_code)
        message(FATAL_ERROR "Failed to checkout tag: '${git_tag}'")
      endif()

      execute_process(
        COMMAND "${git_executable}" submodule update --recursive
        WORKING_DIRECTORY "${module_dir}"
        RESULT_VARIABLE error_code
        )
      if(error_code)
        message(FATAL_ERROR "Failed to update submodules in: '${module_dir}'")
      endif()
    endif()
  endif()
endfunction(_fetch_with_git)

# Fetch a remote module.
#
# A new CMake option is created, Fetch_${module_name}, which defaults to OFF.
# Once set to ON, the module is downloaded into the Remote module group, and an
# option to build the module will be available on the next CMake configuration.
#
# A module name and description are required.  The description will show up in
# the CMake user interface.
#
# The following options are currently supported:
#    [GIT_REPOSITORY url]        # URL of git repo
#    [GIT_TAG tag]               # Git branch name, commit id or tag
function(itk_fetch_module _name _description)
  option(Fetch_${_name} "${_description}" OFF)
  mark_as_advanced(Fetch_${_name})
  if(Fetch_${_name})
    include(CMakeParseArguments)
    cmake_parse_arguments(_fetch_options "" "GIT_REPOSITORY;GIT_TAG" "" ${ARGN})
    find_package(Git)
    if(NOT GIT_EXECUTABLE)
      message(FATAL_ERROR "error: could not find git for clone of ${_name}")
    endif()
    _fetch_with_git("${GIT_EXECUTABLE}"
      "${_fetch_options_GIT_REPOSITORY}"
      "${_fetch_options_GIT_TAG}"
      "${ITK_SOURCE_DIR}/Modules/Remote/${_name}"
      )
  endif()
endfunction()
