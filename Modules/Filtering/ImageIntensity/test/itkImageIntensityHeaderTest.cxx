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

#include "itkAbsImageFilter.h"
#include "itkAcosImageFilter.h"
#include "itkAdaptImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkAndImageFilter.h"
#include "itkAsinImageFilter.h"
#include "itkAtan2ImageFilter.h"
#include "itkAtanImageFilter.h"
#include "itkBinaryMagnitudeImageFilter.h"
#include "itkBoundedReciprocalImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkComplexToModulusImageFilter.h"
#include "itkComplexToPhaseImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkConstrainedValueAdditionImageFilter.h"
#include "itkConstrainedValueDifferenceImageFilter.h"
#include "itkCosImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkEdgePotentialImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkExpNegativeImageFilter.h"
#include "itkHistogramMatchingImageFilter.txx"
#include "itkIntensityWindowingImageFilter.txx"
#include "itkInvertIntensityImageFilter.txx"
#include "itkJoinImageFilter.h"
#include "itkJoinSeriesImageFilter.txx"
#include "itkLog10ImageFilter.h"
#include "itkLogImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkMatrixIndexSelectionImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkMinimumImageFilter.h"
#include "itkModulusImageFilter.txx"
#include "itkMultiplyImageFilter.h"
#include "itkNaryAddImageFilter.h"
#include "itkNaryFunctorImageFilter.txx"
#include "itkNaryMaximumImageFilter.h"
#include "itkNormalizeImageFilter.txx"
#include "itkNotImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkPolylineMask2DImageFilter.txx"
#include "itkPolylineMaskImageFilter.txx"
#include "itkRGBToLuminanceImageFilter.h"
#include "itkRescaleIntensityImageFilter.txx"
#include "itkShiftScaleImageFilter.txx"
#include "itkSigmoidImageFilter.h"
#include "itkSinImageFilter.h"
#include "itkSqrtImageFilter.h"
#include "itkSquareImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"
#include "itkTanImageFilter.h"
#include "itkTernaryAddImageFilter.h"
#include "itkTernaryMagnitudeImageFilter.h"
#include "itkTernaryMagnitudeSquaredImageFilter.h"
#include "itkVectorExpandImageFilter.txx"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkVectorRescaleIntensityImageFilter.h"
#include "itkVectorRescaleIntensityImageFilter.txx"
#include "itkWeightedAddImageFilter.h"
#include "itkXorImageFilter.h"


int itkImageIntensityHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
