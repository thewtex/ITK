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

#include "itkAnchorCloseImageFilter.h"
#include "itkAnchorDilateImageFilter.h"
#include "itkAnchorErodeDilateImageFilter.h"
#include "itkAnchorErodeDilateImageFilter.txx"
#include "itkAnchorErodeDilateLine.txx"
#include "itkAnchorErodeImageFilter.h"
#include "itkAnchorOpenCloseImageFilter.txx"
#include "itkAnchorOpenCloseLine.txx"
#include "itkAnchorOpenImageFilter.h"
#include "itkAnchorUtilities.txx"
#include "itkBasicDilateImageFilter.txx"
#include "itkBasicErodeImageFilter.txx"
#include "itkBinaryBallStructuringElement.txx"
#include "itkBinaryCrossStructuringElement.txx"
#include "itkBinaryDilateImageFilter.txx"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryErodeImageFilter.txx"
#include "itkBinaryMorphologyImageFilter.txx"
#include "itkBinaryPruningImageFilter.txx"
#include "itkBinaryThinningImageFilter.txx"
#include "itkBlackTopHatImageFilter.txx"
#include "itkClosingByReconstructionImageFilter.txx"
#include "itkDilateObjectMorphologyImageFilter.h"
#include "itkDilateObjectMorphologyImageFilter.txx"
#include "itkDoubleThresholdImageFilter.txx"
#include "itkErodeObjectMorphologyImageFilter.txx"
#include "itkFastIncrementalBinaryDilateImageFilter.h"
#include "itkFlatStructuringElement.txx"
#include "itkGrayscaleConnectedClosingImageFilter.txx"
#include "itkGrayscaleConnectedOpeningImageFilter.txx"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.txx"
#include "itkGrayscaleErodeImageFilter.txx"
#include "itkGrayscaleFillholeImageFilter.txx"
#include "itkGrayscaleFunctionDilateImageFilter.txx"
#include "itkGrayscaleFunctionErodeImageFilter.txx"
#include "itkGrayscaleGeodesicDilateImageFilter.txx"
#include "itkGrayscaleGeodesicErodeImageFilter.txx"
#include "itkGrayscaleGrindPeakImageFilter.txx"
#include "itkGrayscaleMorphologicalClosingImageFilter.txx"
#include "itkGrayscaleMorphologicalOpeningImageFilter.txx"
#include "itkHConcaveImageFilter.txx"
#include "itkHConvexImageFilter.txx"
#include "itkHMaximaImageFilter.txx"
#include "itkHMinimaImageFilter.h"
#include "itkHMinimaImageFilter.txx"
#include "itkKernelImageFilter.h"
#include "itkKernelImageFilter.txx"
#include "itkMorphologicalGradientImageFilter.txx"
#include "itkMorphologyHistogram.h"
#include "itkMorphologyImageFilter.txx"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkMovingHistogramImageFilter.txx"
#include "itkMovingHistogramImageFilterBase.txx"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkMovingHistogramMorphologyImageFilter.h"
#include "itkMovingHistogramMorphologyImageFilter.txx"
#include "itkObjectMorphologyImageFilter.txx"
#include "itkOpeningByReconstructionImageFilter.txx"
#include "itkReconstructionByDilationImageFilter.h"
#include "itkReconstructionByErosionImageFilter.h"
#include "itkReconstructionImageFilter.txx"
#include "itkSharedMorphologyUtilities.h"
#include "itkSharedMorphologyUtilities.txx"
#include "itkVanHerkGilWermanDilateImageFilter.h"
#include "itkVanHerkGilWermanErodeDilateImageFilter.txx"
#include "itkVanHerkGilWermanErodeImageFilter.h"
#include "itkVanHerkGilWermanUtilities.h"
#include "itkVanHerkGilWermanUtilities.txx"
#include "itkWhiteTopHatImageFilter.h"
#include "itkWhiteTopHatImageFilter.txx"


int itkMathematicalMorphologyHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
