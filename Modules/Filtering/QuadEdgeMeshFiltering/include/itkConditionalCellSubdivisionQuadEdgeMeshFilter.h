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

#ifndef __itkConditionalCellSubdivisionQuadEdgeMeshFilter_h
#define __itkConditionalCellSubdivisionQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**
 * \class ConditionalCellSubdivisionQuadEdgeMeshFilter
 *
 * \brief FIXME
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template< typename TInputMesh, typename TCellSubdivisionFilter, typename TCriterion >
class ConditionalCellSubdivisionQuadEdgeMeshFilter:
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, typename TCellSubdivisionFilter::OutputMeshType >
{
public:
  typedef ConditionalCellSubdivisionQuadEdgeMeshFilter                  Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, typename TCellSubdivisionFilter::OutputMeshType> Superclass;
  typedef SmartPointer< Self >                                        Pointer;
  typedef SmartPointer< const Self >                                  ConstPointer;

  typedef TCellSubdivisionFilter                                                     CellSubdivisionFilterType;
  typedef typename CellSubdivisionFilterType::Pointer                                CellSubdivisionFilterPointer;

  typedef TInputMesh                                                                 InputMeshType;
  typedef typename InputMeshType::Pointer                                            InputMeshPointer;

  typedef typename CellSubdivisionFilterType::OutputMeshType                         OutputMeshType;
  typedef typename OutputMeshType::Pointer                                           OutputMeshPointer;

  typedef typename CellSubdivisionFilterType::OutputCellIdentifier                   OutputCellIdentifier;
  typedef typename CellSubdivisionFilterType::OutputCellIdentifierListType           OutputCellIdentifierListType;
  typedef typename CellSubdivisionFilterType::OutputCellIdentifierListConstIterator  OutputCellIdentifierListConstIterator;

  typedef TCriterion                                                                 CriterionType;
  typedef typename CriterionType::Pointer                                            CriterionPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro(ConditionalCellSubdivisionQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter);
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( SameTypeCheck,
    ( Concept::SameType< typename CellSubdivisionFilterType::InputMeshType, typename CellSubdivisionFilterType::OutputMeshType > ) );
  itkConceptMacro( SameTypeCheckMesh,
    ( Concept::SameType< typename CellSubdivisionFilterType::OutputMeshType, typename CriterionType::MeshType > ) );
  itkConceptMacro( SameTypeCheckContainer,
    ( Concept::SameType< typename CellSubdivisionFilterType::OutputCellIdentifierListType, typename CriterionType::CellIdContainer > ) );
#endif

  itkSetObjectMacro(SubdivisionCriterion, CriterionType);

protected:
  ConditionalCellSubdivisionQuadEdgeMeshFilter();

  virtual ~ConditionalCellSubdivisionQuadEdgeMeshFilter() {}

  virtual void GenerateData();

  void PrintSelf(std::ostream & os, Indent indent) const;

  CellSubdivisionFilterPointer  m_CellSubdivisionFilter;
  OutputCellIdentifierListType  m_CellsToBeSubdivided;
  CriterionPointer              m_SubdivisionCriterion;

private:
  ConditionalCellSubdivisionQuadEdgeMeshFilter(const Self &); // purposely not implemented
  void operator=(const Self &);                // purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConditionalCellSubdivisionQuadEdgeMeshFilter.hxx"
#endif

#endif
