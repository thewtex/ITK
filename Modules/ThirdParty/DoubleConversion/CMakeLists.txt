project(ITKDOUBLECONVERSION)
set(ITKDoubleConversion_THIRD_PARTY 1)

option(ITK_USE_SYSTEM_DOUBLECONVERSION
  "Use an outside build of double-conversion" OFF)

mark_as_advanced(ITK_USE_SYSTEM_DOUBLECONVERSION)

if(ITK_USE_SYSTEM_DOUBLECONVERSION)
  find_package(double-conversion NO_MODULE)
  set(ITKDOUBLECONVERSION_SYSTEM_INCLUDE_DIRS
    ${double-conversion_INCLUDE_DIRS})
  set(ITKDOUBLECONVERSION_LIBRARIES
    "${double-conversion_LIBRARIES}")
else()
  set(ITKDOUBLECONVERSION_INCLUDE_DIRS
    ${ITKDOUBLECONVERSION_SOURCE_DIR}/src/double-conversion)
  set(ITKDOUBLECONVERSION_LIBRARIES itkdouble-conversion)
endif()

itk_module_impl()
