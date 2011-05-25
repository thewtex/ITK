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

#include "itkBorderQuadEdgeMeshFilter.txx"
#include "itkCleanQuadEdgeMeshFilter.h"
#include "itkDecimationQuadEdgeMeshFilter.h"
#include "itkDelaunayConformingQuadEdgeMeshFilter.txx"
#include "itkDiscreteCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscreteCurvatureTensorQuadEdgeMeshFilter.h"
#include "itkDiscreteGaussianCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscreteMaximumCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscreteMeanCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscreteMinimumCurvatureQuadEdgeMeshFilter.h"
#include "itkDiscretePrincipalCurvaturesQuadEdgeMeshFilter.h"
#include "itkEdgeDecimationQuadEdgeMeshFilter.txx"
#include "itkNormalQuadEdgeMeshFilter.txx"
#include "itkParameterizationQuadEdgeMeshFilter.h"
#include "itkParameterizationQuadEdgeMeshFilter.txx"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkQuadEdgeMeshDecimationQuadricElementHelper.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkQuadricDecimationQuadEdgeMeshFilter.h"
#include "itkSmoothingQuadEdgeMeshFilter.txx"
#include "itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter.txx"


int itkQuadEdgeMeshFilteringHeaderTest ( int , char * [] )
{
  return EXIT_SUCCESS;
}
