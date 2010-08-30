add_library( itkNetlibSlatec
  # from SLATEC/FNLIB
  d1mach.c
  d9lgmc.c
  d9gmit.c
  d9lgic.c
  d9lgit.c
  dbetai.c
  dcsevl.c
  dgami.c
  dgamit.c
  dgamlm.c
  dgamma.c
  dgamr.c
  dlbeta.c
  dlgams.c
  dlngam.c
  dlnrel.c
  initds.c
  xermsg.c
  d_int.c
  d_mod.c
 )

if(ITK_USE_SYSTEM_VXL)
  target_link_libraries( itkNetlibSlatec v3p_netlib netlib)
else(ITK_USE_SYSTEM_VXL)
  target_link_libraries( itkNetlibSlatec itkv3p_netlib )
endif(ITK_USE_SYSTEM_VXL)

if(ITK_LIBRARY_PROPERTIES)
  set_target_properties(itkNetlibSlatec PROPERTIES ${ITK_LIBRARY_PROPERTIES})
endif(ITK_LIBRARY_PROPERTIES)

if(NOT ITK_INSTALL_NO_LIBRARIES)
  install(TARGETS itkNetlibSlatec
    RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
endif(NOT ITK_INSTALL_NO_LIBRARIES)

if(BORLAND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w-8004 -w-8057")
endif(BORLAND)
