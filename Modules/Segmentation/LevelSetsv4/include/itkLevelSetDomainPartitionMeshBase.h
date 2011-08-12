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
#ifndef __itkLevelSetDomainPartitionMeshBase_h
#define __itkLevelSetDomainPartitionMeshBase_h

#include "itkLevelSetDomainPartitionBase.h"
#include <map>

namespace itk
{
/** \class LevelSetDomainPartitionMeshBase
 *
 * \brief Helper class used to partition domain and efficiently compute overlap.
 *  \ingroup ITKLevelSetsv4
 */
template< class TMesh >
class LevelSetDomainPartitionMeshBase :
  public LevelSetDomainPartitionBase< TMesh >
{
public:

  typedef LevelSetDomainPartitionMeshBase       Self;
  typedef LevelSetDomainPartitionBase< TMesh >  Superclass;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  itkStaticConstMacro( PointDimension, unsigned int, TMesh::PointDimension);

  itkTypeMacro( LevelSetDomainPartitionMeshBase,
                LevelSetDomainPartitionBase );

  typedef TMesh                                     MeshType;
  typedef typename MeshType::Pointer                MeshPointer;
  typedef typename MeshType::ConstPointer           MeshConstPointer;
  typedef typename MeshType::PointType              PointType;
  typedef typename MeshType::PointIdentifierType    PointIdentifierType;
  typedef typename MeshType::PointsContainerPointer PointsContainerPointer;
  typedef typename MeshType::PointsContainerConstIterator
                                                    PointsContainerConstIterator;

  typedef typename Superclass::IdentifierListType IdentifierListType;

  typedef std::map< PointIdentifierType, IdentifierListType > ListMeshType;

  void SetMesh( MeshPointer iMesh )
  {
    m_Mesh = iMesh;
  }

protected:
  LevelSetDomainPartitionMeshBase() : Superclass(), m_Mesh( NULL )
  {}

  virtual ~LevelSetDomainPartitionMeshBase(){}

  MeshPointer     m_Mesh;
  ListMeshType    m_ListDomain;

  /** Populate a list mesh with each node being a list of overlapping
   *  level set support at that pixel */
  virtual void PopulateListDomain()
    {
    PointsContainerPointer points = m_Mesh->GetPoints();
    PointsContainerConstIterator p_it = points->Begin();
    PointsContainerConstIterator p_end = points->End();

    while( p_it != p_end )
      {
      PointIdentifierType idx = p_it->Index();
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

  /** Allocate a list mesh with each node being a list of overlapping
   *  level set support at that pixel */
  void AllocateListDomain()
  {}

private:
  LevelSetDomainPartitionMeshBase(const Self &); //purposely not
                                                       // implemented
  void operator=(const Self &);                        //purposely not
                                                       // implemented
};
} //end namespace itk

#endif
