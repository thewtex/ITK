IF (ITK_USE_GPU)

  project(ITK-GPUCommon)

  FIND_PACKAGE ( OpenCL REQUIRED )
  IF ( NOT ${OPENCL_FOUND} )
     MESSAGE ( FATAL "Could not find OpenCL" )
  ENDIF ()

  FILE(GLOB itkGPUSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cl)
  FILE(GLOB itkGPUHDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
  FILE(GLOB itkTxxSRC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.txx)

  SOURCE_GROUP("OpenCL Source Files" FILES ${itkGPUSRC})

  # OpenCL source file location - using relative path to make the code portable
  FILE(COPY ${itkGPUSRC} DESTINATION ${ITK_BINARY_DIR}/bin/OpenCL)
  FILE(COPY ${itkGPUSRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/OpenCL)

  MESSAGE( "OpenCL source files to copy : " ${itkGPUSRC})

  #configure_file(
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/pathToOpenCLSourceCode.h.in
  #  pathToOpenCLSourceCode.h)

  set(ITK-GPUCommon_LIBRARIES ITK-GPUCommon)

  itk_module_impl()

ENDIF(ITK_USE_GPU)
