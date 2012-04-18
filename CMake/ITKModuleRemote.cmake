# Functions to fetch external modules.

include(ExternalProject)

function(itk_fetch_module _name _description)
  option(Fetch_${_name} "${_description}" OFF)
  mark_as_advanced(Fetch_${_name})
  if(Fetch_${_name})
    ExternalProject_Add(${_name}
      DOWNLOAD_DIR "${ITK_SOURCE_DIR}/Modules/Remote/${_name}"
      ${ARGN}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      TEST_COMMAND ""
      )
  endif()
endfunction()
