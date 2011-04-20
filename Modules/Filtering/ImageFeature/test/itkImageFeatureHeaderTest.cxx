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

#include "itkBilateralImageFilter.hpp"
#include "itkSobelEdgeDetectionImageFilter.hpp"
#include "itkDerivativeImageFilter.h"
#include "itkSimpleContourExtractorImageFilter.hpp"
#include "itkLaplacianSharpeningImageFilter.hpp"
#include "itkHessianRecursiveGaussianImageFilter.hpp"
#include "itkGradientVectorFlowImageFilter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.hpp"
#include "itkHoughTransform2DCirclesImageFilter.hpp"
#include "itkCannyEdgeDetectionImageFilter.hpp"
#include "itkDerivativeImageFilter.hpp"
#include "itkZeroCrossingImageFilter.hpp"
#include "itkHessian3DToVesselnessMeasureImageFilter.hpp"
#include "itkHoughTransform2DLinesImageFilter.h"
#include "itkZeroCrossingBasedEdgeDetectionImageFilter.hpp"
#include "itkGradientVectorFlowImageFilter.hpp"
#include "itkLaplacianImageFilter.hpp"
#include "itkHoughTransform2DLinesImageFilter.hpp"
#include "itkTensorRelativeAnisotropyImageFilter.h"
#include "itkTensorFractionalAnisotropyImageFilter.h"



int itkImageFeatureHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
