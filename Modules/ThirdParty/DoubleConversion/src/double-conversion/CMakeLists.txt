
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

if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(itkdouble-conversion PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif()

itk_module_target(itkdouble-conversion NO_INSTALL)

if (BUILD_SHARED_LIBS)
  set_target_properties(double-conversion
    PROPERTIES VERSION ${double-conversion_SOVERSION}
               SOVERSION ${double-conversion_SOVERSION_MAJOR})
endif()

#
# install command to set up library install
# given the above PUBLIC_HEADER property set, this
# pulls along all the header files with the library.
if(NOT ITK_INSTALL_NO_LIBRARIES)
  install(TARGETS itkdouble-conversion
    EXPORT ${ITKDoubleConversion-targets}
    RUNTIME DESTINATION "${ITKDoubleConversion_INSTALL_RUNTIME_DIR}" COMPONENT Applications
    LIBRARY DESTINATION "${ITKDoubleConversion_INSTALL_LIBRARY_DIR}" COMPONENT Libraries
    ARCHIVE DESTINATION "${ITKDoubleConversion_INSTALL_LIBRARY_DIR}" COMPONENT Development)
endif()

if(NOT ITK_INSTALL_NO_DEVELOPMENT)
  install(FILES ${headers}
    DESTINATION "${ITKDoubleConversion_INSTALL_INCLUDE_DIR}" COMPONENT Development)
endif()
