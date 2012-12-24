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

#ifndef __itkConditionalEdgeCellSubdivisionQuadEdgeMeshFilter_h
#define __itkConditionalEdgeCellSubdivisionQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**
 * \class ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter
 *
 * \brief FIXME
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template< typename TInputMesh, typename TEdgeCellSubdivisionFilter, typename TCriterion >
class ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter:
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, typename TEdgeCellSubdivisionFilter::OutputMeshType >
{
public:
  typedef ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter                           Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, typename TEdgeCellSubdivisionFilter::OutputMeshType> Superclass;
  typedef SmartPointer< Self >                                                       Pointer;
  typedef SmartPointer< const Self >                                                 ConstPointer;

  typedef TEdgeCellSubdivisionFilter                                                 EdgeCellSubdivisionFilterType;
  typedef typename EdgeCellSubdivisionFilterType::Pointer                            EdgeCellSubdivisionFilterPointer;

  typedef TInputMesh                                                                 InputMeshType;
  typedef typename InputMeshType::Pointer                                            InputMeshPointer;

  typedef typename EdgeCellSubdivisionFilterType::OutputMeshType                     OutputMeshType;
  typedef typename OutputMeshType::Pointer                                           OutputMeshPointer;

  typedef typename EdgeCellSubdivisionFilterType::InputQEType                        InputQEType;
  typedef typename EdgeCellSubdivisionFilterType::InputEdgeListType                  InputEdgeListType;
  typedef typename EdgeCellSubdivisionFilterType::InputEdgeListConstIterator         InputEdgeListConstIterator;

  typedef TCriterion                                                                 CriterionType;
  typedef typename CriterionType::Pointer                                            CriterionPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro(ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter);
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( SameTypeCheck,
    ( Concept::SameType< typename EdgeCellSubdivisionFilterType::InputMeshType, typename EdgeCellSubdivisionFilterType::OutputMeshType > ) );
  itkConceptMacro( SameTypeCheckMesh,
    ( Concept::SameType< typename EdgeCellSubdivisionFilterType::OutputMeshType, typename CriterionType::MeshType > ) );
#endif

  itkSetObjectMacro(SubdivisionCriterion, CriterionType);

protected:
  ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter();

  virtual ~ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter() {}

  virtual void GenerateData();

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  ConditionalEdgeCellSubdivisionQuadEdgeMeshFilter(const Self &); // purposely not implemented
  void operator=(const Self &);                // purposely not implemented

protected:
  EdgeCellSubdivisionFilterPointer  m_EdgeCellSubdivisionFilter;
  InputEdgeListType                 m_EdgesToBeSubdivided;
  CriterionPointer                  m_SubdivisionCriterion;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConditionalEdgeCellSubdivisionQuadEdgeMeshFilter.hxx"
#endif

#endif
