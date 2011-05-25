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

#include "itkBinaryMedialNodeMetric.txx"
#include "itkBloxBoundaryPointImage.h"
#include "itkBloxBoundaryPointImage.txx"
#include "itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilter.txx"
#include "itkBloxBoundaryPointItem.txx"
#include "itkBloxBoundaryPointPixel.h"
#include "itkBloxBoundaryPointPixel.txx"
#include "itkBloxBoundaryPointToCoreAtomImageFilter.h"
#include "itkBloxBoundaryPointToCoreAtomImageFilter.txx"
#include "itkBloxBoundaryProfileImage.txx"
#include "itkBloxBoundaryProfileImageToBloxCoreAtomImageFilter.txx"
#include "itkBloxBoundaryProfileItem.h"
#include "itkBloxBoundaryProfileItem.txx"
#include "itkBloxBoundaryProfilePixel.txx"
#include "itkBloxCoreAtomImage.txx"
#include "itkBloxCoreAtomItem.txx"
#include "itkBloxCoreAtomPixel.txx"
#include "itkBloxImage.txx"
#include "itkBloxItem.h"
#include "itkBloxPixel.txx"
#include "itkCoreAtomImageToDistanceMatrixProcess.txx"
#include "itkCoreAtomImageToUnaryCorrespondenceMatrixProcess.txx"
#include "itkCorrespondenceDataStructure.txx"
#include "itkCorrespondingList.txx"
#include "itkCorrespondingMedialNodeClique.txx"
#include "itkGradientImageToBloxBoundaryPointImageFilter.txx"
#include "itkMedialNodePairCorrespondenceProcess.txx"
#include "itkMedialNodeTripletCorrespondenceProcess.txx"
#include "itkNodeList.txx"
#include "itkSecondaryNodeList.txx"
#include "itkUnaryMedialNodeMetric.h"
#include "itkUnaryMedialNodeMetric.txx"


int itkBloxHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
