project(ITKGPUCommon)


IF (ITK_USE_GPU)
  FIND_PACKAGE ( OpenCL REQUIRED )
  IF ( NOT ${OPENCL_FOUND} )
     MESSAGE ( FATAL "Could not find OpenCL" )
  ENDIF ()

  FILE(GLOB itkGPUSRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cl)
  FILE(GLOB itkGPUHDR ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
  FILE(GLOB itkTxxSRC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.txx)

  SOURCE_GROUP("OpenCL Source Files" FILES ${itkGPUSRC})

  FILE(COPY ${itkGPUSRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/OpenCL)
  MESSAGE( "OpenCL files to copy " ${itkGPUSRC} )


  #configure_file(
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/pathToOpenCLSourceCode.h.in
  #  pathToOpenCLSourceCode.h)

  set(ITKGPUCommon_LIBRARIES ITKGPUCommon)
  itk_module_impl()

ENDIF(ITK_USE_GPU)
