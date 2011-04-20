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

#include "itkInterpolateImageFilter.hpp"
#include "itkPadImageFilter.hpp"
#include "itkConstantPadImageFilter.hpp"
#include "itkPasteImageFilter.hpp"
#include "itkBSplineCenteredL2ResampleImageFilterBase.h"
#include "itkShrinkImageFilter.hpp"
#include "itkWarpImageFilter.hpp"
#include "itkFlipImageFilter.hpp"
#include "itkBSplineUpsampleImageFilter.hpp"
#include "itkResampleImageFilter.hpp"
#include "itkBSplineDecompositionImageFilter.hpp"
#include "itkExpandImageFilter.hpp"
#include "itkCropImageFilter.h"
#include "itkWarpVectorImageFilter.hpp"
#include "itkOrientImageFilter.hpp"
#include "itkBSplineCenteredResampleImageFilterBase.hpp"
#include "itkInterpolateImagePointsFilter.hpp"
#include "itkRegionOfInterestImageFilter.hpp"
#include "itkBSplineResampleImageFilterBase.hpp"
#include "itkMirrorPadImageFilter.hpp"
#include "itkVectorResampleImageFilter.hpp"
#include "itkWrapPadImageFilter.hpp"
#include "itkBSplineDecompositionImageFilter.h"
#include "itkTileImageFilter.hpp"
#include "itkBSplineCenteredL2ResampleImageFilterBase.hpp"
#include "itkRegionOfInterestImageFilter.h"
#include "itkPermuteAxesImageFilter.hpp"
#include "itkBSplineCenteredResampleImageFilterBase.h"
#include "itkChangeInformationImageFilter.hpp"
#include "itkCropImageFilter.hpp"
#include "itkBSplineL2ResampleImageFilterBase.hpp"
#include "itkVectorResampleImageFilter.h"
#include "itkBSplineDownsampleImageFilter.hpp"



int itkImageGridHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
