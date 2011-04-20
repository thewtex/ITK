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

#include "itkBinaryThresholdImageFunction.hpp"
//BUG 11905
//#include "itkBSplineInterpolateImageFunction.hpp"
//#include "itkBSplineResampleImageFunction.h"
#include "itkCentralDifferenceImageFunction.hpp"
#include "itkCovarianceImageFunction.hpp"
#include "itkExtrapolateImageFunction.h"
#include "itkGaussianBlurImageFunction.hpp"
#include "itkGaussianDerivativeImageFunction.hpp"
#include "itkImageFunction.hpp"
#include "itkInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.hpp"
#include "itkMahalanobisDistanceThresholdImageFunction.hpp"
#include "itkMeanImageFunction.hpp"
#include "itkMedianImageFunction.hpp"
#include "itkNearestNeighborExtrapolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkNeighborhoodBinaryThresholdImageFunction.hpp"
#include "itkNeighborhoodOperatorImageFunction.hpp"
#include "itkRayCastInterpolateImageFunction.hpp"
#include "itkScatterMatrixImageFunction.hpp"
#include "itkSumOfSquaresImageFunction.hpp"
#include "itkVarianceImageFunction.hpp"
#include "itkVectorInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.hpp"
#include "itkVectorMeanImageFunction.hpp"
#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.hpp"



int itkImageFunctionHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
