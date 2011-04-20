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

#include "itkAccumulateImageFilter.hpp"
#include "itkAdaptiveHistogramEqualizationImageFilter.hpp"
#include "itkBinaryProjectionImageFilter.h"
#include "itkGetAverageSliceImageFilter.hpp"
#include "itkHistogramAlgorithmBase.hpp"
//BUG  11909
//#include "itkImageMomentsCalculator.hpp"
#include "itkImagePCADecompositionCalculator.hpp"
#include "itkImagePCAShapeModelEstimator.hpp"
#include "itkImageShapeModelEstimatorBase.hpp"
#include "itkLabelStatisticsImageFilter.hpp"
#include "itkMaximumProjectionImageFilter.h"
#include "itkMeanProjectionImageFilter.h"
#include "itkMedianProjectionImageFilter.h"
#include "itkMinimumMaximumImageFilter.hpp"
#include "itkMinimumProjectionImageFilter.h"
#include "itkNormalizedCorrelationImageFilter.hpp"
#include "itkProjectionImageFilter.hpp"
#include "itkStandardDeviationProjectionImageFilter.h"
#include "itkStatisticsImageFilter.hpp"
#include "itkSumProjectionImageFilter.h"



int itkImageStatisticsHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
