#
# Examples on the use of segmentation algorithms
#

project(ImageSegmentation)
include_regular_expression("^.*$")

foreach(example
    ConfidenceConnected
    ConfidenceConnected3D
    NeighborhoodConnectedImageFilter
    IsolatedConnectedImageFilter
    ConnectedThresholdImageFilter
    FastMarchingImageFilter
    GeodesicActiveContourShapePriorLevelSetImageFilter
    GibbsPriorImageFilter1
    HoughTransform2DLinesImageFilter
    HoughTransform2DCirclesImageFilter
    VectorConfidenceConnected
    RelabelComponentImageFilter
    CannySegmentationLevelSetImageFilter
    GeodesicActiveContourImageFilter
    ShapeDetectionLevelSetFilter
    CurvesLevelSetImageFilter
    WatershedSegmentation1
    WatershedSegmentation2
    CellularSegmentation1
    CellularSegmentation2
    LaplacianSegmentationLevelSetImageFilter
    ThresholdSegmentationLevelSetImageFilter
    )
  add_example(${example})
endforeach()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
