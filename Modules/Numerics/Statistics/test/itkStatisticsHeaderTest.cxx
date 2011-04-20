/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkChiSquareDistribution.h"
#include "itkCovarianceSampleFilter.hpp"
#include "itkDecisionRule.h"
#include "itkDecisionRuleBase.h"
#include "itkDenseFrequencyContainer2.h"
#include "itkDistanceMetric.hpp"
#include "itkDistanceToCentroidMembershipFunction.hpp"
#include "itkEuclideanDistanceMetric.hpp"
#include "itkEuclideanSquareDistanceMetric.hpp"
#include "itkExpectationMaximizationMixtureModelEstimator.hpp"
#include "itkGaussianDistribution.h"
#include "itkGaussianMembershipFunction.hpp"
#include "itkGaussianMixtureModelComponent.hpp"
#include "itkHistogram.hpp"
#include "itkHistogramToEntropyImageFilter.h"
#include "itkHistogramToImageFilter.hpp"
#include "itkHistogramToIntensityImageFilter.h"
#include "itkHistogramToLogProbabilityImageFilter.h"
#include "itkHistogramToProbabilityImageFilter.h"
#include "itkHistogramToTextureFeaturesFilter.hpp"
#include "itkImageClassifierFilter.hpp"
#include "itkImageToHistogramFilter.hpp"
#include "itkImageToListSampleAdaptor.hpp"
#include "itkImageToListSampleFilter.hpp"
#include "itkJointDomainImageToListSampleAdaptor.hpp"
#include "itkKalmanLinearEstimator.h"
#include "itkKdTree.hpp"
#include "itkKdTreeBasedKmeansEstimator.hpp"
#include "itkKdTreeGenerator.hpp"
#include "itkListSample.hpp"
#include "itkMahalanobisDistanceMembershipFunction.hpp"
#include "itkMahalanobisDistanceMetric.hpp"
#include "itkManhattanDistanceMetric.hpp"
#include "itkMaximumDecisionRule.h"
#include "itkMaximumDecisionRule2.h"
#include "itkMaximumRatioDecisionRule2.h"
#include "itkMeanSampleFilter.hpp"
#include "itkMeasurementVectorTraits.h"
#include "itkMembershipFunctionBase.h"
#include "itkMembershipSample.hpp"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkMinimumDecisionRule.h"
#include "itkMinimumDecisionRule2.h"
#include "itkMixtureModelComponentBase.hpp"
#include "itkNeighborhoodSampler.hpp"
#include "itkNormalVariateGenerator.h"
#include "itkPointSetToListSampleAdaptor.hpp"
#include "itkProbabilityDistribution.h"
#include "itkRandomVariateGeneratorBase.h"
#include "itkSample.h"
#include "itkSampleClassifierFilter.hpp"
#include "itkSampleToHistogramFilter.hpp"
#include "itkSampleToSubsampleFilter.hpp"
#include "itkScalarImageToCooccurrenceListSampleFilter.hpp"
#include "itkScalarImageToCooccurrenceMatrixFilter.hpp"
#include "itkScalarImageToHistogramGenerator.hpp"
#include "itkScalarImageToTextureFeaturesFilter.hpp"
#include "itkSparseFrequencyContainer2.h"
#include "itkStandardDeviationPerComponentSampleFilter.hpp"
#include "itkStatisticsAlgorithm.hpp"
#include "itkSubsample.hpp"
#include "itkTDistribution.h"
#include "itkWeightedCentroidKdTreeGenerator.hpp"
#include "itkWeightedCovarianceSampleFilter.hpp"
#include "itkWeightedMeanSampleFilter.hpp"



int itkStatisticsHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
