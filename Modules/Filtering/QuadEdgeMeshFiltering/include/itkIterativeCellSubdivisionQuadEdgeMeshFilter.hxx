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

#ifndef __itkIterativeCellSubdivisionQuadEdgeMeshFilter_hxx
#define __itkIterativeCellSubdivisionQuadEdgeMeshFilter_hxx

#include "itkIterativeCellSubdivisionQuadEdgeMeshFilter.h"

namespace itk
{
template< typename TInputMesh, typename TCellSubdivisionFilter >
IterativeCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::IterativeCellSubdivisionQuadEdgeMeshFilter()
{
  m_CellSubdivisionFilter = CellSubdivisionFilterType::New();
  m_ResolutionLevels = 1;
}

template< typename TInputMesh, typename TCellSubdivisionFilter >
void
IterativeCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::GenerateData()
{
  this->CopyInputMeshToOutputMeshGeometry();

  this->m_CellSubdivisionFilter->SetCellsToBeSubdivided( this->m_CellsToBeSubdivided );
  while ( m_ResolutionLevels != 0 )
    {
    this->m_CellSubdivisionFilter->SetInput( this->GetOutput() );
    this->m_CellSubdivisionFilter->Update();
    OutputMeshPointer mesh = this->m_CellSubdivisionFilter->GetOutput();
    mesh->DisconnectPipeline();
    this->GraftOutput( mesh );
    --m_ResolutionLevels;
    }
}

template< typename TInputMesh, typename TCellSubdivisionFilter >
void
IterativeCellSubdivisionQuadEdgeMeshFilter< TInputMesh, TCellSubdivisionFilter >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  std::cout << indent << "Subdivision Resolution Levels: " << m_ResolutionLevels << std::endl;
}
}
#endif
