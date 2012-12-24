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

#ifndef __itkIterativeCellSubdivisionQuadEdgeMeshFilter_h
#define __itkIterativeCellSubdivisionQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**
 * \class IterativeCellSubdivisionQuadEdgeMeshFilter
 *
 * \brief FIXME
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template< typename TInputMesh, typename TCellSubdivisionFilter >
class IterativeCellSubdivisionQuadEdgeMeshFilter:
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, typename TCellSubdivisionFilter::OutputMeshType >
{
public:
  typedef IterativeCellSubdivisionQuadEdgeMeshFilter                  Self;
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

  /** Run-time type information (and related methods).   */
  itkTypeMacro(IterativeCellSubdivisionQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter);
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( SameTypeCheck,
    ( Concept::SameType< typename CellSubdivisionFilterType::InputMeshType, typename CellSubdivisionFilterType::OutputMeshType > ) );
#endif

  itkSetMacro(ResolutionLevels, unsigned int);
  itkGetConstMacro(ResolutionLevels, unsigned int);

  itkGetConstReferenceMacro(CellsToBeSubdivided, OutputCellIdentifierListType);
  void SetCellsToBeSubdivided(const OutputCellIdentifierListType & cellIdList)
    {
    m_CellsToBeSubdivided = cellIdList;
    this->Modified();
    }

  void AddSubdividedCellId(OutputCellIdentifier cellId){m_CellsToBeSubdivided.push_back(cellId);}

protected:
  IterativeCellSubdivisionQuadEdgeMeshFilter();

  virtual ~IterativeCellSubdivisionQuadEdgeMeshFilter() {}

  virtual void GenerateData();

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  IterativeCellSubdivisionQuadEdgeMeshFilter(const Self &); // purposely not implemented
  void operator=(const Self &);                // purposely not implemented

protected:
  CellSubdivisionFilterPointer        m_CellSubdivisionFilter;
  OutputCellIdentifierListType        m_CellsToBeSubdivided;
  unsigned int                        m_ResolutionLevels;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIterativeCellSubdivisionQuadEdgeMeshFilter.hxx"
#endif

#endif
