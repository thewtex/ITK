
# Make it easier to enable the main supported languages, by providing the option even when
# the Wrapping directory has not yet been included
option(ITK_WRAP_PYTHON "Build python support" OFF)

option(ITK_WRAPPING "Build external languages support" OFF)
mark_as_advanced(ITK_WRAPPING)

# check whether we should go in the wrapping folder, even with ITK_WRAPPING is OFF
if(NOT ITK_WRAPPING_REACHED)
  if(ITK_WRAP_PYTHON OR ITK_WRAP_JAVA)
    # force ITK_WRAPPING to ON
    unset(ITK_WRAPPING CACHE)
    option(ITK_WRAPPING "Build external languages support" ON)
    mark_as_advanced(ITK_WRAPPING)
  endif()
endif()

if(ITK_WRAPPING)
  if(NOT ITK_BUILD_SHARED_LIBS)
    message(WARNING "Wrapping requires a shared build, changing BUILD_SHARED_LIBS to ON")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build ITK with shared libraries." FORCE )
  endif()
  if(NOT ITK_LEGACY_REMOVE)
    message(WARNING "Wrapping requires deprecated code is removed: changing ITK_LEGACY_REMOVE to ON")
    set(ITK_LEGACY_REMOVE ON CACHE BOOL "Remove all legacy code." FORCE )
  endif()
endif()
