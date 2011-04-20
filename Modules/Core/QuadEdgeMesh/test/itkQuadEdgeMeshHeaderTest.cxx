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

#include "itkGeometricalQuadEdge.hpp"
#include "itkQuadEdge.h"
#include "itkQuadEdgeCellTraitsInfo.h"
#include "itkQuadEdgeMesh.hpp"
#include "itkQuadEdgeMeshBaseIterator.h"
#include "itkQuadEdgeMeshBoundaryEdgesMeshFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorCreateCenterVertexFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorDeleteCenterVertexFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorJoinFacetFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorJoinVertexFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorSplitEdgeFunction.h"
#include "itkQuadEdgeMeshEulerOperatorSplitFacetFunction.hpp"
#include "itkQuadEdgeMeshEulerOperatorSplitVertexFunction.hpp"
#include "itkQuadEdgeMeshExtendedTraits.h"
#include "itkQuadEdgeMeshFrontIterator.hpp"
#include "itkQuadEdgeMeshFunctionBase.h"
#include "itkQuadEdgeMeshLineCell.hpp"
#include "itkQuadEdgeMeshMacro.h"
#include "itkQuadEdgeMeshPoint.hpp"
#include "itkQuadEdgeMeshPolygonCell.hpp"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.hpp"
#include "itkQuadEdgeMeshTopologyChecker.hpp"
#include "itkQuadEdgeMeshTraits.h"
#include "itkQuadEdgeMeshZipMeshFunction.hpp"



int itkQuadEdgeMeshHeaderTest ( int , char * [] )
{

  return EXIT_SUCCESS;
}
