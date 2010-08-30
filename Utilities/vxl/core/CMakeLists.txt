project( vxlcore )

set(global_sources
 vxl_copyright.h
 vxl_version.h
 vxl_config.h.in
)

if(NOT VXL_INSTALL_NO_DEVELOPMENT)
  INSTALL_NOBASE_HEADER_FILES(${VXL_INSTALL_ROOT}/core ${global_sources})
endif(NOT VXL_INSTALL_NO_DEVELOPMENT)

# common test executable
subdirs(testlib)

# numerics
subdirs(vnl)
