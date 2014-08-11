project(StatisticsExamples)

foreach(example
    BayesianPluginClassifier
    BayesianClassifierInitializer
    BayesianClassifier
    EuclideanDistanceMetric
    ExpectationMaximizationMixtureModelEstimator
    GaussianMembershipFunction
    Histogram
    KdTree
    KdTreeBasedKMeansClustering
    ListSample
    SampleToHistogramFilter
    MaximumDecisionRule
    MaximumRatioDecisionRule
    MembershipSample
    MinimumDecisionRule
    NormalVariateGenerator
    PointSetToListSampleAdaptor
    SampleStatistics
    SampleSorting
    WeightedSampleStatistics
    ImageHistogram1
    ImageHistogram2
    ImageHistogram3
    ImageHistogram4
    ScalarImageMarkovRandomField1
    ScalarImageKmeansClassifier
    ImageToListSampleAdaptor
    ImageEntropy1
    ImageMutualInformation1
    ScalarImageKmeansModelEstimator
    )
  add_example(${example})
endforeach()

#
# At the moment, itkNeighborhoodSampler is incomplete, so this example
# can't be updated
#
#add_executable(NeighborhoodSampler NeighborhoodSampler.cxx )
#target_link_libraries(NeighborhoodSampler ${ITK_LIBRARIES})

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
