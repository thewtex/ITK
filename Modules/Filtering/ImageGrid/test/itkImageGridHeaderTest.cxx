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

#include "itkBSplineCenteredL2ResampleImageFilterBase.h"
#include "itkBSplineCenteredL2ResampleImageFilterBase.txx"
#include "itkBSplineCenteredResampleImageFilterBase.h"
#include "itkBSplineCenteredResampleImageFilterBase.txx"
#include "itkBSplineDecompositionImageFilter.h"
#include "itkBSplineDecompositionImageFilter.txx"
#include "itkBSplineDownsampleImageFilter.txx"
#include "itkBSplineL2ResampleImageFilterBase.txx"
#include "itkBSplineResampleImageFilterBase.txx"
#include "itkBSplineUpsampleImageFilter.txx"
#include "itkChangeInformationImageFilter.txx"
#include "itkConstantPadImageFilter.txx"
#include "itkCropImageFilter.h"
#include "itkCropImageFilter.txx"
#include "itkExpandImageFilter.txx"
#include "itkFlipImageFilter.txx"
#include "itkInterpolateImageFilter.txx"
#include "itkInterpolateImagePointsFilter.txx"
#include "itkMirrorPadImageFilter.txx"
#include "itkOrientImageFilter.txx"
#include "itkPadImageFilter.txx"
#include "itkPasteImageFilter.txx"
#include "itkPermuteAxesImageFilter.txx"
#include "itkRegionOfInterestImageFilter.h"
#include "itkRegionOfInterestImageFilter.txx"
#include "itkResampleImageFilter.txx"
#include "itkShrinkImageFilter.txx"
#include "itkTileImageFilter.txx"
#include "itkVectorResampleImageFilter.h"
#include "itkVectorResampleImageFilter.txx"
#include "itkWarpImageFilter.txx"
#include "itkWarpVectorImageFilter.txx"
#include "itkWrapPadImageFilter.txx"


int itkImageGridHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
