set(ITKIOMINC_SRC
     itkMincHelpers.cxx
     itkMincImageIO.cxx
     itkMincImageIOFactory.cxx
     itkMincTransformIO.cxx
     itkMincTransformIOFactory.cxx
     itkMincHelpers.cxx
)
INCLUDE_DIRECTORIES(${MINC2_INCLUDE_DIRS})


ADD_LIBRARY(ITKIOMINC ${ITKIOMINC_SRC})
TARGET_LINK_LIBRARIES(ITKIOMINC  ${ITKIOMINC_LIBRARIES} ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOMINC)
