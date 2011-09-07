set(VideoCoreCommon_SRC
itkTemporalRegion.cxx
itkTemporalDataObject.cxx
itkTemporalProcessObject.cxx
)

add_library(VideoCoreCommon ${VideoCoreCommon_SRC})
target_link_libraries(VideoCoreCommon ${ITKCommon_LIBRARIES})
itk_module_target(VideoCoreCommon)
