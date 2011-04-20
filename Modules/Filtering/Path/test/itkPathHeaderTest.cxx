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

#include "itkPolyLineParametricPath.hpp"
#include "itkOrthogonalSwath2DPathFilter.hpp"
#include "itkPathToChainCodePathFilter.hpp"
#include "itkPathIterator.hpp"
#include "itkPathFunctions.h"
#include "itkImageAndPathToImageFilter.hpp"
#include "itkPathSource.hpp"
#include "itkPathToPathFilter.hpp"
#include "itkChainCodePath2D.h"
#include "itkChainCodePath.hpp"
#include "itkPath.hpp"
#include "itkExtractOrthogonalSwath2DImageFilter.hpp"
#include "itkFourierSeriesPath.h"
#include "itkPathToImageFilter.hpp"
#include "itkOrthogonallyCorrected2DParametricPath.h"
#include "itkParametricPath.h"
#include "itkChainCodeToFourierSeriesPathFilter.hpp"
#include "itkParametricPath.hpp"
#include "itkPathAndImageToPathFilter.hpp"
#include "itkFourierSeriesPath.hpp"
#include "itkPathConstIterator.hpp"
#include "itkPathSource.h"
#include "itkPath.h"



int itkPathHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
