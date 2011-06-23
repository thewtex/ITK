project(ITK_KWSys)

set(KWSYS_HEADER_ROOT ${ITK_KWSys_BINARY_DIR}/src)
set(ITK_KWSys_INCLUDE_DIRS
  ${KWSYS_HEADER_ROOT}
  )
set(ITK_KWSys_LIBRARIES itksys)
itk_module_impl()
