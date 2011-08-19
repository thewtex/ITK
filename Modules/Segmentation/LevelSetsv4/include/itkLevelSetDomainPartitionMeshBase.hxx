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
#ifndef __itkLevelSetDomainPartitionMeshBase_hxx
#define __itkLevelSetDomainPartitionMeshBase_hxx

#include "itkLevelSetDomainPartitionMeshBase.h"

namespace itk
{
template< class TMesh >
LevelSetDomainPartitionMeshBase <TMesh>
::LevelSetDomainPartitionMeshBase() : m_Mesh( NULL )
{
}

template< class TMesh >
LevelSetDomainPartitionMeshBase <TMesh>
::~LevelSetDomainPartitionMeshBase()
{
}

template< class TMesh >
void
LevelSetDomainPartitionMeshBase <TMesh>
::SetMesh( const MeshPointer iMesh )
{
  this->m_Mesh = iMesh;
}

template< class TMesh >
void
LevelSetDomainPartitionMeshBase <TMesh>
::PopulateListDomain()
{
  PointsContainerConstPointer points = m_Mesh->GetPoints();
  PointsContainerConstIterator p_it = points->Begin();
  PointsContainerConstIterator p_end = points->End();

  while( p_it != p_end )
    {
    PointIdentifierType & idx = p_it->Index();
    IdentifierListType L;

    for( unsigned int i = 0; i < this->m_FunctionCount; ++i )
      {
      if ( this->m_LevelSetDataPointerVector[i]->VerifyInsideRegion( idx ) )
        {
        L.push_back(i);
        }

      m_ListDomain[ idx ] = L;
      ++p_it;
      }
    }
}

template< class TMesh >
void
LevelSetDomainPartitionMeshBase <TMesh>
::AllocateListDomain()
{
}

} //end namespace itk

#endif
