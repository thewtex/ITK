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

#include "itkGrayscaleFunctionDilateImageFilter.hpp"
#include "itkMorphologyHistogram.h"
#include "itkBlackTopHatImageFilter.hpp"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryMorphologyImageFilter.hpp"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicErodeImageFilter.hpp"
#include "itkReconstructionImageFilter.hpp"
#include "itkHMaximaImageFilter.hpp"
#include "itkMorphologyImageFilter.hpp"
#include "itkWhiteTopHatImageFilter.hpp"
#include "itkBinaryDilateImageFilter.hpp"
#include "itkErodeObjectMorphologyImageFilter.hpp"
#include "itkClosingByReconstructionImageFilter.hpp"
#include "itkGrayscaleGrindPeakImageFilter.hpp"
#include "itkGrayscaleGeodesicDilateImageFilter.hpp"
#include "itkAnchorErodeDilateLine.hpp"
#include "itkVanHerkGilWermanDilateImageFilter.h"
#include "itkDoubleThresholdImageFilter.hpp"
#include "itkGrayscaleConnectedOpeningImageFilter.hpp"
#include "itkReconstructionByDilationImageFilter.h"
#include "itkMorphologicalGradientImageFilter.hpp"
#include "itkAnchorErodeDilateImageFilter.hpp"
#include "itkGrayscaleFunctionErodeImageFilter.hpp"
#include "itkHConcaveImageFilter.hpp"
#include "itkGrayscaleFillholeImageFilter.hpp"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkAnchorOpenImageFilter.h"
#include "itkAnchorOpenCloseImageFilter.hpp"
#include "itkBinaryCrossStructuringElement.hpp"
#include "itkAnchorErodeImageFilter.h"
#include "itkMovingHistogramImageFilterBase.hpp"
#include "itkReconstructionByErosionImageFilter.h"
#include "itkMovingHistogramImageFilter.hpp"
#include "itkVanHerkGilWermanUtilities.h"
#include "itkGrayscaleErodeImageFilter.hpp"
#include "itkHMinimaImageFilter.h"
#include "itkBinaryThinningImageFilter.hpp"
#include "itkDilateObjectMorphologyImageFilter.h"
#include "itkObjectMorphologyImageFilter.hpp"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.hpp"
#include "itkVanHerkGilWermanUtilities.hpp"
#include "itkFastIncrementalBinaryDilateImageFilter.h"
#include "itkAnchorUtilities.hpp"
#include "itkDilateObjectMorphologyImageFilter.hpp"
#include "itkMovingHistogramMorphologyImageFilter.h"
#include "itkAnchorOpenCloseLine.hpp"
#include "itkHConvexImageFilter.hpp"
#include "itkHMinimaImageFilter.hpp"
#include "itkWhiteTopHatImageFilter.h"
#include "itkKernelImageFilter.h"
#include "itkVanHerkGilWermanErodeImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.hpp"
#include "itkKernelImageFilter.hpp"
#include "itkOpeningByReconstructionImageFilter.hpp"
#include "itkBasicDilateImageFilter.hpp"
#include "itkGrayscaleGeodesicErodeImageFilter.hpp"
#include "itkSharedMorphologyUtilities.hpp"
#include "itkVanHerkGilWermanErodeDilateImageFilter.hpp"
#include "itkAnchorCloseImageFilter.h"
#include "itkFlatStructuringElement.hpp"
#include "itkSharedMorphologyUtilities.h"
#include "itkAnchorErodeDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.hpp"
#include "itkMovingHistogramMorphologyImageFilter.hpp"
#include "itkGrayscaleConnectedClosingImageFilter.hpp"
#include "itkGrayscaleDilateImageFilter.hpp"
#include "itkBinaryPruningImageFilter.hpp"
#include "itkBinaryErodeImageFilter.hpp"
#include "itkAnchorDilateImageFilter.h"



int itkMathematicalMorphologyHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
