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

#include "itkBloxImage.hpp"
#include "itkBloxPixel.hpp"
#include "itkBloxItem.h"
#include "itkBinaryMedialNodeMetric.hpp"
#include "itkBloxCoreAtomItem.hpp"
#include "itkCoreAtomImageToDistanceMatrixProcess.hpp"
#include "itkCorrespondingList.hpp"
#include "itkBloxBoundaryPointToCoreAtomImageFilter.h"
#include "itkMedialNodeTripletCorrespondenceProcess.hpp"
#include "itkBloxBoundaryProfileImageToBloxCoreAtomImageFilter.hpp"
#include "itkBloxBoundaryPointItem.hpp"
#include "itkCorrespondenceDataStructure.hpp"
#include "itkBloxCoreAtomImage.hpp"
#include "itkBloxBoundaryPointImage.hpp"
#include "itkUnaryMedialNodeMetric.h"
#include "itkSecondaryNodeList.hpp"
#include "itkBloxCoreAtomPixel.hpp"
#include "itkBloxBoundaryPointPixel.hpp"
#include "itkGradientImageToBloxBoundaryPointImageFilter.hpp"
#include "itkCorrespondingMedialNodeClique.hpp"
#include "itkUnaryMedialNodeMetric.hpp"
#include "itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilter.hpp"
#include "itkNodeList.hpp"
#include "itkBloxBoundaryPointToCoreAtomImageFilter.hpp"
#include "itkBloxBoundaryProfileItem.h"
#include "itkBloxBoundaryPointImage.h"
#include "itkBloxBoundaryPointPixel.h"
#include "itkMedialNodePairCorrespondenceProcess.hpp"
#include "itkBloxBoundaryProfileImage.hpp"
#include "itkBloxBoundaryProfilePixel.hpp"
#include "itkCoreAtomImageToUnaryCorrespondenceMatrixProcess.hpp"
#include "itkBloxBoundaryProfileItem.hpp"



int itkBloxHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
