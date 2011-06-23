set(ITK_IO_Base_SRC
itkImageIORegion.cxx
itkArchetypeSeriesFileNames.cxx
itkImageIOFactory.cxx
itkIOCommon.cxx
itkNumericSeriesFileNames.cxx
itkImageIOBase.cxx
itkRegularExpressionSeriesFileNames.cxx
itkStreamingImageIOBase.cxx
)

add_library(ITK_IO_Base ${ITK_IO_Base_SRC})
target_link_libraries(ITK_IO_Base  ${ITK_Common_LIBRARIES})
itk_module_target(ITK_IO_Base)
