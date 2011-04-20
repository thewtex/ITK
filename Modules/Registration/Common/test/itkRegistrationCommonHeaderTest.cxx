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

#include "itkCommandVnlIterationUpdate.h"
#include "itkPDEDeformableRegistrationFunction.h"
#include "itkPointSetToPointSetMetric.h"
#include "itkMeanSquaresPointSetToImageMetric.h"
#include "itkMeanSquaresHistogramImageToImageMetric.hpp"
#include "itkPointSetToImageRegistrationMethod.hpp"
#include "itkImageToImageMetric.hpp"
#include "itkPointSetToSpatialObjectDemonsRegistration.hpp"
#include "itkNormalizedCorrelationPointSetToImageMetric.hpp"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkKullbackLeiblerCompareHistogramImageToImageMetric.hpp"
#include "itkPointSetToImageMetric.hpp"
#include "itkImageRegistrationMethodImageSource.h"
#include "itkHistogramImageToImageMetric.hpp"
#include "itkMeanSquaresPointSetToImageMetric.hpp"
#include "itkSimpleMultiResolutionImageRegistrationUI.h"
#include "itkMatchCardinalityImageToImageMetric.hpp"
#include "itkMeanSquareRegistrationFunction.hpp"
#include "itkGradientDifferenceImageToImageMetric.hpp"
#include "itkPointSetToPointSetRegistrationMethod.hpp"
#include "itkKappaStatisticImageToImageMetric.hpp"
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.hpp"
#include "itkImageToSpatialObjectRegistrationMethod.h"
#include "itkMutualInformationImageToImageMetric.hpp"
#include "itkMultiResolutionImageRegistrationMethod.hpp"
#include "itkCorrelationCoefficientHistogramImageToImageMetric.h"
#include "itkEuclideanDistancePointMetric.hpp"
#include "itkRecursiveMultiResolutionPyramidImageFilter.hpp"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkNormalizedCorrelationImageToImageMetric.hpp"
#include "itkMeanReciprocalSquareDifferenceImageToImageMetric.hpp"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.hpp"
#include "itkCommandIterationUpdate.h"
#include "itkMultiResolutionPyramidImageFilter.hpp"
#include "itkImageRegistrationMethod.hpp"
#include "itkMutualInformationHistogramImageToImageMetric.hpp"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.hpp"
#include "itkPointSetToPointSetMetric.hpp"
#include "itkCompareHistogramImageToImageMetric.hpp"
#include "itkImageToSpatialObjectRegistrationMethod.hpp"
#include "itkImageToSpatialObjectMetric.hpp"
#include "itkCorrelationCoefficientHistogramImageToImageMetric.hpp"
#include "itkMeanReciprocalSquareDifferencePointSetToImageMetric.hpp"



int itkRegistrationCommonHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
