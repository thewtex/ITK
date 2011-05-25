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

#include "itkChainCodePath.txx"
#include "itkChainCodePath2D.h"
#include "itkChainCodeToFourierSeriesPathFilter.txx"
#include "itkExtractOrthogonalSwath2DImageFilter.txx"
#include "itkFourierSeriesPath.h"
#include "itkFourierSeriesPath.txx"
#include "itkImageAndPathToImageFilter.txx"
#include "itkOrthogonalSwath2DPathFilter.txx"
#include "itkOrthogonallyCorrected2DParametricPath.h"
#include "itkParametricPath.h"
#include "itkParametricPath.txx"
#include "itkPath.h"
#include "itkPath.txx"
#include "itkPathAndImageToPathFilter.txx"
#include "itkPathConstIterator.txx"
#include "itkPathFunctions.h"
#include "itkPathIterator.txx"
#include "itkPathSource.h"
#include "itkPathSource.txx"
#include "itkPathToChainCodePathFilter.txx"
#include "itkPathToImageFilter.txx"
#include "itkPathToPathFilter.txx"
#include "itkPolyLineParametricPath.txx"


int itkPathHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
