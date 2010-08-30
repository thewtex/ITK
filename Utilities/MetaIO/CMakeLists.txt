#=============================================================================
# MetaIO
# Copyright 2000-2010 Insight Software Consortium
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
if(VTK_SOURCE_DIR)

  set(METAIO_FOR_VTK 1)
  mark_as_advanced( METAIO_FOR_VTK )
  set(METAIO_NAMESPACE "vtkmetaio")

else(VTK_SOURCE_DIR)

  set(METAIO_FOR_ITK 1)
  mark_as_advanced( METAIO_FOR_ITK )
  set(METAIO_NAMESPACE "ITKMetaIO")

  if(ITK_USE_LIBXML2)
    add_definitions(-DMETAIO_USE_LIBXML2)
    include_directories(${ITK_LIBXML2_INCLUDE_DIR})
  endif(ITK_USE_LIBXML2)

endif(VTK_SOURCE_DIR)

project( ${METAIO_NAMESPACE} )

include_directories(${${METAIO_NAMESPACE}_SOURCE_DIR})

add_library(${METAIO_NAMESPACE}
  metaTypes.h
  metaUtils.cxx
  metaArrow.cxx
  metaBlob.cxx
  metaCommand.h
  metaCommand.cxx
  metaContour.h
  metaContour.cxx
  metaDTITube.cxx
  metaEllipse.cxx
  metaGroup.cxx
  metaGaussian.cxx
  metaImage.cxx
  metaImageUtils.cxx
  metaLandmark.cxx
  metaLine.cxx
  metaMesh.cxx
  metaMesh.h
  metaObject.cxx
  metaOutput.h
  metaOutput.cxx
  metaScene.cxx
  metaSurface.cxx
  metaTube.cxx
  metaVesselTube.cxx
  metaTransform.cxx
  metaTubeGraph.cxx
  metaForm.h
  metaForm.cxx
  metaArray.h
  metaArray.cxx
)

include_regular_expression("^.*$")

# Need nsl to resolve gethostbyname on SunOS-5.8
# and socket also
if(CMAKE_SYSTEM MATCHES "SunOS.*")
  target_link_libraries(${METAIO_NAMESPACE} socket nsl)
endif(CMAKE_SYSTEM MATCHES "SunOS.*")

if(METAIO_FOR_VTK)
  if(BUILD_SHARED_LIBS)
    add_definitions(-Dvtkmetaio_BUILD_SHARED_LIBS)
  endif(BUILD_SHARED_LIBS)
  target_link_libraries(${METAIO_NAMESPACE} ${VTK_ZLIB_LIBRARIES}
                                            ${KWSYS_NAMESPACE})
  if(VTK_LIBRARY_PROPERTIES)
    set_target_properties(${METAIO_NAMESPACE}
                          PROPERTIES ${VTK_LIBRARY_PROPERTIES})
  endif(VTK_LIBRARY_PROPERTIES)
else(METAIO_FOR_VTK)
  if(METAIO_FOR_ITK)
    target_link_libraries(${METAIO_NAMESPACE} ${ITK_ZLIB_LIBRARIES}
                                              ${KWSYS_NAMESPACE})
    if(ITK_USE_LIBXML2)
      target_link_libraries(${METAIO_NAMESPACE} ${ITK_LIBXML2_LIBRARIES})
    endif(ITK_USE_LIBXML2)

    if(ITK_LIBRARY_PROPERTIES)
      set_target_properties(${METAIO_NAMESPACE}
                            PROPERTIES ${ITK_LIBRARY_PROPERTIES})
    endif(ITK_LIBRARY_PROPERTIES)
  else(METAIO_FOR_ITK)
    target_link_libraries(${METAIO_NAMESPACE} ${ZLIB_LIBRARIES}
                                              ${KWSYS_NAMESPACE})
  endif(METAIO_FOR_ITK)
endif(METAIO_FOR_VTK)

if(WIN32)
  if(BORLAND)
    target_link_libraries(${METAIO_NAMESPACE} import32)
  else(BORLAND)
    target_link_libraries(${METAIO_NAMESPACE} comctl32 wsock32)
  endif(BORLAND)
endif(WIN32)


#subdirs(tests)

if(METAIO_FOR_VTK)
  if(NOT VTK_INSTALL_NO_LIBRARIES)
    install(TARGETS ${METAIO_NAMESPACE}
            RUNTIME DESTINATION ${VTK_INSTALL_BIN_DIR_CM24}
                    COMPONENT RuntimeLibraries
            LIBRARY DESTINATION ${VTK_INSTALL_LIB_DIR_CM24}
                    COMPONENT RuntimeLibraries
            ARCHIVE DESTINATION ${VTK_INSTALL_LIB_DIR_CM24}
                    COMPONENT Development
            )
    #TODO: Install Copyright.txt
    #install(FILES Copyright.txt
    #        DESTINATION ${VTK_INSTALL_DOC_DIR_CM24}/vtkmetaio
    #        COMPONENT RuntimeLibraries)
  endif(NOT VTK_INSTALL_NO_LIBRARIES)
  if(NOT VTK_INSTALL_NO_DEVELOPMENT)
    file(GLOB __files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${__files}
                  DESTINATION ${VTK_INSTALL_INCLUDE_DIR_CM24}/vtkmetaio
                  COMPONENT Development)
  endif(NOT VTK_INSTALL_NO_DEVELOPMENT)
else(METAIO_FOR_VTK)
  if(METAIO_FOR_ITK)
    if(NOT ITK_INSTALL_NO_LIBRARIES)
      install(TARGETS ${METAIO_NAMESPACE}
              RUNTIME DESTINATION ${ITK_INSTALL_BIN_DIR_CM24}
                      COMPONENT RuntimeLibraries
              LIBRARY DESTINATION ${ITK_INSTALL_LIB_DIR_CM24}
                      COMPONENT RuntimeLibraries
              ARCHIVE DESTINATION ${ITK_INSTALL_LIB_DIR_CM24}
                      COMPONENT Development
              )
      #TODO: Install Copyright.txt
    endif(NOT ITK_INSTALL_NO_LIBRARIES)
    if(NOT ITK_INSTALL_NO_DEVELOPMENT)
      file(GLOB __files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
      install(FILES ${__files}
              DESTINATION ${ITK_INSTALL_INCLUDE_DIR_CM24}/Utilities/MetaIO
                          COMPONENT Development)
    endif(NOT ITK_INSTALL_NO_DEVELOPMENT)
  else(METAIO_FOR_ITK)
    string(REGEX REPLACE "^/" "" INSTALL_LIB_DIR_CM24 "${INSTALL_LIB_DIR}")
    string(REGEX REPLACE "^/" "" INSTALL_BIN_DIR_CM24 "${INSTALL_BIN_DIR}")
    string(REGEX REPLACE "^/" "" INSTALL_INCLUDE_DIR_CM24
           "${INSTALL_INCLUDE_DIR}")
    install(TARGETS ${METAIO_NAMESPACE}
            RUNTIME DESTINATION ${INSTALL_BIN_DIR_CM24}
                    COMPONENT RuntimeLibraries
            LIBRARY DESTINATION ${INSTALL_LIB_DIR_CM24}
                    COMPONENT RuntimeLibraries
            ARCHIVE DESTINATION ${INSTALL_LIB_DIR_CM24}
                    COMPONENT Development
            )
    #TODO: Install Copyright.txt
    file(GLOB __files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${__files}
                  DESTINATION ${INSTALL_INCLUDE_DIR_CM24}/kwMetaIO
                  COMPONENT Development)
  endif(METAIO_FOR_ITK)
endif(METAIO_FOR_VTK)



