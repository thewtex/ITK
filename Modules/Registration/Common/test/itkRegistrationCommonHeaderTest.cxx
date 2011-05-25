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

#include "itkCommandIterationUpdate.h"
#include "itkCommandVnlIterationUpdate.h"
#include "itkCompareHistogramImageToImageMetric.txx"
#include "itkCorrelationCoefficientHistogramImageToImageMetric.h"
#include "itkCorrelationCoefficientHistogramImageToImageMetric.txx"
#include "itkEuclideanDistancePointMetric.txx"
#include "itkGradientDifferenceImageToImageMetric.txx"
#include "itkHistogramImageToImageMetric.txx"
#include "itkImageRegistrationMethod.h"
#include "itkImageRegistrationMethod.txx"
#include "itkImageRegistrationMethodImageSource.h"
#include "itkImageToImageMetric.txx"
#include "itkImageToSpatialObjectMetric.txx"
#include "itkImageToSpatialObjectRegistrationMethod.h"
#include "itkImageToSpatialObjectRegistrationMethod.txx"
#include "itkKappaStatisticImageToImageMetric.txx"
#include "itkKullbackLeiblerCompareHistogramImageToImageMetric.txx"
#include "itkMatchCardinalityImageToImageMetric.txx"
#include "itkMattesMutualInformationImageToImageMetric.txx"
#include "itkMeanReciprocalSquareDifferenceImageToImageMetric.txx"
#include "itkMeanReciprocalSquareDifferencePointSetToImageMetric.txx"
#include "itkMeanSquareRegistrationFunction.txx"
#include "itkMeanSquaresHistogramImageToImageMetric.txx"
#include "itkMeanSquaresImageToImageMetric.txx"
#include "itkMeanSquaresPointSetToImageMetric.h"
#include "itkMeanSquaresPointSetToImageMetric.txx"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionImageRegistrationMethod.txx"
#include "itkMultiResolutionPyramidImageFilter.txx"
#include "itkMutualInformationHistogramImageToImageMetric.txx"
#include "itkMutualInformationImageToImageMetric.txx"
#include "itkNormalizedCorrelationImageToImageMetric.txx"
#include "itkNormalizedCorrelationPointSetToImageMetric.txx"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.h"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.txx"
#include "itkPDEDeformableRegistrationFunction.h"
#include "itkPointSetToImageMetric.txx"
#include "itkPointSetToImageRegistrationMethod.txx"
#include "itkPointSetToPointSetMetric.h"
#include "itkPointSetToPointSetMetric.txx"
#include "itkPointSetToPointSetRegistrationMethod.txx"
#include "itkPointSetToSpatialObjectDemonsRegistration.txx"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.txx"
#include "itkSimpleMultiResolutionImageRegistrationUI.h"


int itkRegistrationCommonHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
