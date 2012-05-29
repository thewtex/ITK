

# if(NOT ITK_USE_SYSTEM_DCMTK)
#   if(NOT WIN32 OR MINGW)
#     set(lib_prefix lib)
#     set(lib_suffix .a)
#   else(NOT WIN32)
#     set(lib_prefix "")
#     set(lib_suffix .lib)
#   endif()

#   set(DCMTK_LIB ${ITK_BINARY_DIR}/Modules/ThirdParty/DCMTK/DCMTK_Prereqs/lib)
#   set(_ITKDCMTK_LIBRARIES dcmdata dcmimage dcmimgle dcmjpeg
#     dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 ofstd
#     oflog)

#   foreach(lib ${_ITKDCMTK_LIBRARIES})
#     add_library(${lib} STATIC IMPORTED)
#     set(libname ${DCMTK_LIB}/${lib_prefix}${lib}${lib_suffix})
#     set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION ${libname})
#   endforeach()
# endif(NOT ITK_USE_SYSTEM_DCMTK)
