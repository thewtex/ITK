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

#ifndef __itkConditionalTriangleCellSubdivisionQuadEdgeMeshFilter_hxx
#define __itkConditionalTriangleCellSubdivisionQuadEdgeMeshFilter_hxx

#include "itkConditionalTriangleCellSubdivisionQuadEdgeMeshFilter.h"

namespace itk
{
template< typename TInputMesh, typename TCellSubdivisionFilter >
ConditionalTriangleCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::ConditionalTriangleCellSubdivisionQuadEdgeMeshFilter()
{
  m_CellSubdivisionFilter = CellSubdivisionFilterType::New();
}

template< typename TInputMesh, typename TCellSubdivisionFilter >
void
ConditionalTriangleCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::SetSubdivisionCriterion( CriterionType * criterion )
{
  m_SubdivisionCriterion = criterion;
  this->Modified();
}

template< typename TInputMesh, typename TCellSubdivisionFilter >
void
ConditionalTriangleCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::GenerateData()
{
  this->CopyInputMeshToOutputMeshGeometry();
  this->m_SubdivisionCriterion->Compute( this->GetOutput(), this->m_CellsToBeSubdivided );

  while( !this->m_CellsToBeSubdivided.empty() )
    {
    this->m_CellSubdivisionFilter->SetInput( this->GetOutput() );
    this->m_CellSubdivisionFilter->SetCellsToBeSubdivided( this->m_CellsToBeSubdivided );
    this->m_CellSubdivisionFilter->Update();
    OutputMeshPointer mesh = this->m_CellSubdivisionFilter->GetOutput();
    mesh->DisconnectPipeline();
    this->GraftOutput( mesh );
    this->m_SubdivisionCriterion->Compute( this->GetOutput(), this->m_CellsToBeSubdivided );
    }
}

template< typename TInputMesh, typename TCellSubdivisionFilter >
void
ConditionalTriangleCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
}
}
#endif
