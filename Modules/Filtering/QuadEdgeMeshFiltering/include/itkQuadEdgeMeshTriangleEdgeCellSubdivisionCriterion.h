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

#ifndef __itkQuadEdgeMeshTriangleEdgeCellSubdivisionCriterion_h
#define __itkQuadEdgeMeshTriangleEdgeCellSubdivisionCriterion_h

#include "itkObject.h"

namespace itk
{
/**
 *\class QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion
 *\brief
 *\ingroup ITKQuadEdgeMeshFiltering
 */
template< typename TTriangleEdgeCellSubdivisionFilter >
class ITK_EXPORT QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion : public Object
{
public:
  typedef QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion                  Self;
  typedef Object                                                            Superclass;
  typedef SmartPointer< Self >                                              Pointer;
  typedef SmartPointer< const Self >                                        ConstPointer;

  typedef typename TTriangleEdgeCellSubdivisionFilter::InputMeshType        MeshType;
  typedef typename MeshType::Pointer                                        MeshPointer;
  typedef typename MeshType::ConstPointer                                   MeshConstPointer;
  typedef typename MeshType::PointsContainerPointer                         PointsContainerPointer;
  typedef typename MeshType::PointsContainerConstIterator                   PointsContainerConstIterator;
  typedef typename MeshType::PointsContainerIterator                        PointsContainerIterator;
  typedef typename MeshType::CellsContainer                                 CellsContainer;
  typedef typename MeshType::CellsContainerPointer                          CellsContainerPointer;
  typedef typename MeshType::CellsContainerIterator                         CellsContainerIterator;
  typedef typename MeshType::CellsContainerConstIterator                    CellsContainerConstIterator;
  typedef typename MeshType::PointType                                      PointType;
  typedef typename MeshType::CoordRepType                                   CoordRepType;
  typedef typename MeshType::PointIdentifier                                PointIdentifier;
  typedef typename MeshType::CellIdentifier                                 CellIdentifier;
  typedef typename MeshType::CellType                                       CellType;
  typedef typename MeshType::QEType                                         QEType;
  typedef typename MeshType::PointIdIterator                                PointIdIterator;
  typedef typename TTriangleEdgeCellSubdivisionFilter::InputEdgeListType    EdgeListType;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion, Object );

  virtual void Compute( MeshType * mesh, EdgeListType & edgeList ) = 0;

protected:
  QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion(){}
  ~QuadEdgeMeshTriangleEdgeCellSubdivisionCriterion(){}
};

}
#endif
