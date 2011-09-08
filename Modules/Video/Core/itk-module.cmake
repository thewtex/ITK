set(DOCUMENTATION "This module contains modules to enable temporal data objects
and temporal process objects. For lack of a better term, we call this the video
module, but it is truly for any sort of data with a time dimension.")

itk_module(ITKVideoCore
  DEPENDS
    ITKCommon
  TEST_DEPENDS
    ITKTestKernel
    ITKIOVideo
  DESCRIPTION
    "${DOCUMENTATION}"
)
