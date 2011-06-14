project(ITKGPUCommon)

IF (ITK_USE_GPU)

 FIND_PACKAGE ( OpenCL REQUIRED )
  IF ( NOT ${OPENCL_FOUND} )
     MESSAGE ( FATAL "Could not find OpenCL" )
  ENDIF ()

  FILE(GLOB itkGPUSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cl)
  FILE(GLOB itkGPUHDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
  FILE(GLOB itkHxxSRC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx)

  SOURCE_GROUP("OpenCL Source Files" FILES ${itkGPUSRC})

  # OpenCL source file location - using relative path to make the code portable
  FILE(COPY ${itkGPUSRC} DESTINATION ${ITK_BINARY_DIR}/bin/OpenCL)
  FILE(COPY ${itkGPUSRC} DESTINATION ${ITK_BINARY_DIR}/bin/../OpenCL)
  FILE(COPY ${itkGPUSRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/OpenCL)

  MESSAGE( "OpenCL source files to copy : " ${itkGPUSRC})

  #configure_file(
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/pathToOpenCLSourceCode.h.in
  #  pathToOpenCLSourceCode.h)

  set(ITKGPUCommon_LIBRARIES ITKGPUCommon)

  itk_module_impl()

ENDIF(ITK_USE_GPU)
