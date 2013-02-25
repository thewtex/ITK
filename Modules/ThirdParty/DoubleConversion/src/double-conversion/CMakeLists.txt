
set(headers
  bignum.h
  cached-powers.h
  diy-fp.h
  double-conversion.h
  fast-dtoa.h
  fixed-dtoa.h
  ieee.h
  strtod.h
  utils.h
  DoubleToString.h
  )

add_library(itkdouble-conversion
bignum.cc
bignum-dtoa.cc
cached-powers.cc
diy-fp.cc
double-conversion.cc
fast-dtoa.cc
fixed-dtoa.cc
strtod.cc
${headers}
)
#
# associates the list of headers with the library
# for the purposes of installation/import into other projects
#
# install command to set up library install
# given the above PUBLIC_HEADER property set, this
# pulls along all the header files with the library.
install(TARGETS itkdouble-conversion
  EXPORT ${DoubleConversion_TARGETS_NAME}
  RUNTIME DESTINATION "${DoubleConversion_INSTALL_BIN_DIR}" COMPONENT Applications
  LIBRARY DESTINATION "${DoubleConversion_INSTALL_LIB_DIR}/lib" COMPONENT Libraries
  ARCHIVE DESTINATION "${DoubleConversion_INSTALL_LIB_DIR}/lib" COMPONENT Development)

install(FILES ${headers} DESTINATION "${DoubleConversion_INSTALL_INCLUDE_DIR}" COMPONENT Headers)
