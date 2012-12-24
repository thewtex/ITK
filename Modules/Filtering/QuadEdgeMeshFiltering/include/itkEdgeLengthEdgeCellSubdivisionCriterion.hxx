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

#ifndef __itkEdgeLengthEdgeCellSubdivisionCriterion_hxx
#define __itkEdgeLengthEdgeCellSubdivisionCriterion_hxx

#include "itkEdgeLengthEdgeCellSubdivisionCriterion.h"

namespace itk
{
template< typename TMesh >
void
EdgeLengthEdgeCellSubdivisionCriterion< TMesh >::
Compute(MeshType * mesh, EdgeListType & edgeList)
{
  edgeList.clear();
  typename MeshType::CellsContainer::ConstPointer edges = mesh->GetEdgeCells();
  if( !edges )
    {
    itkExceptionMacro("<<Input mesh has no edges");
    }

  PointType pointArray[2];
  for(typename MeshType::CellsContainer::ConstIterator eter = edges->Begin(); eter != edges->End(); ++eter)
    {
    typename MeshType::EdgeCellType * edge = dynamic_cast<typename MeshType::EdgeCellType *>(eter.Value());
    if( edge )
      {
      mesh->GetPoint( edge->PointIdsBegin()[0], &pointArray[0] );
      mesh->GetPoint( edge->PointIdsBegin()[1], &pointArray[1] );
      if( pointArray[1].EuclideanDistanceTo( pointArray[0] ) > m_MaximumLength )
        {
        edgeList.push_back( mesh->FindEdge( edge->PointIdsBegin()[0], edge->PointIdsBegin()[1]) );
        }
      }
    }
}

}
#endif
