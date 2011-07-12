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
#ifndef __itkQuadEdgeMeshToQuadEdgeMeshFilter_h
#define __itkQuadEdgeMeshToQuadEdgeMeshFilter_h

#include "itkMeshToMeshFilter.h"

namespace itk
{
/** \class QuadEdgeMeshToQuadEdgeMeshFilter
 *  \brief Duplicates the content of a Mesh
 *
 * \author Alexandre Gouaillard, Leonardo Florez-Valencia, Eric Boix
 *
 * This implementation was contributed as a paper to the Insight Journal
 * http://hdl.handle.net/1926/306
 *
 * \ingroup ITK-QuadEdgeMesh
 */
template< typename TInputMesh, typename TOutputMesh >
class ITK_EXPORT QuadEdgeMeshToQuadEdgeMeshFilter:
  public MeshToMeshFilter< TInputMesh, TOutputMesh >
{
public:
  /** Basic types. */
  typedef QuadEdgeMeshToQuadEdgeMeshFilter            Self;
  typedef MeshToMeshFilter< TInputMesh, TOutputMesh > Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Input types. */
  typedef TInputMesh                              InputMeshType;
  typedef typename InputMeshType::Pointer         InputMeshPointer;
  typedef typename InputMeshType::ConstPointer    InputMeshConstPointer;
  typedef typename InputMeshType::CoordRepType    InputCoordRepType;
  typedef typename InputMeshType::PointType       InputPointType;
  typedef typename InputMeshType::PointIdentifier InputPointIdentifier;
  typedef typename InputMeshType::QEPrimal        InputQEPrimal;
  typedef typename InputMeshType::VectorType      InputVectorType;

  typedef typename InputMeshType::PointDataContainer InputPointDataContainer;
  typedef typename InputMeshType::CellDataContainer  InputCellDataContainer;

  typedef typename InputPointDataContainer::ConstPointer
  InputPointDataContainerConstPointer;
  typedef typename InputMeshType::PointsContainerConstIterator
  InputPointsContainerConstIterator;
  typedef typename InputMeshType::PointsContainerConstPointer
  InputPointsContainerConstPointer;
  typedef typename InputMeshType::CellsContainerConstIterator
  InputCellsContainerConstIterator;
  typedef typename InputMeshType::CellsContainerConstPointer
  InputCellsContainerConstPointer;

  typedef typename InputMeshType::EdgeCellType    InputEdgeCellType;
  typedef typename InputMeshType::PolygonCellType InputPolygonCellType;
  typedef typename InputMeshType::PointIdList     InputPointIdList;
  typedef typename InputMeshType::CellTraits      InputCellTraits;
  typedef typename InputCellTraits::PointIdInternalIterator
  InputPointsIdInternalIterator;

  typedef typename InputQEPrimal::IteratorGeom InputQEIterator;

  /** Output types. */
  typedef TOutputMesh                              OutputMeshType;
  typedef typename OutputMeshType::Pointer         OutputMeshPointer;
  typedef typename OutputMeshType::ConstPointer    OutputMeshConstPointer;
  typedef typename OutputMeshType::CoordRepType    OutputCoordRepType;
  typedef typename OutputMeshType::PointType       OutputPointType;
  typedef typename OutputMeshType::PointIdentifier OutputPointIdentifier;
  typedef typename OutputMeshType::QEPrimal        OutputQEPrimal;
  typedef typename OutputMeshType::VectorType      OutputVectorType;
  typedef typename OutputQEPrimal::IteratorGeom    OutputQEIterator;
  typedef typename OutputMeshType::PointsContainerIterator
  OutputPointsContainerIterator;
  typedef typename OutputMeshType::PointsContainerPointer
  OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerConstPointer
  OutputPointsContainerConstPointer;

  typedef typename OutputMeshType::PointDataContainer OutputPointDataContainer;
  typedef typename OutputMeshType::CellDataContainer  OutputCellDataContainer;
public:
  itkNewMacro(Self);
  itkTypeMacro(QuadEdgeMeshToQuadEdgeMeshFilter, MeshToMeshFilter);
protected:
  QuadEdgeMeshToQuadEdgeMeshFilter();
  virtual ~QuadEdgeMeshToQuadEdgeMeshFilter() {}

  virtual void CopyMeshToMesh(const TInputMesh *in, TOutputMesh *out);

  virtual void CopyInputMeshToOutputMesh();

  virtual void CopyInputMeshToOutputMeshGeometry();

  virtual void CopyInputMeshToOutputMeshPoints();

  virtual void CopyInputMeshToOutputMeshCells();

  virtual void CopyInputMeshToOutputMeshEdgeCells();

  virtual void CopyInputMeshToOutputMeshFieldData();

  virtual void CopyInputMeshToOutputMeshPointData();

  virtual void CopyInputMeshToOutputMeshCellData();

private:
  QuadEdgeMeshToQuadEdgeMeshFilter(const Self &); // Not impl.
  void operator=(const Self &);                   // Not impl.
};

//
// Helper functions that copy selected pieces of a Mesh.
// These functions should be templated here in order to
// facilitate their reuse in multiple scenarios.
//

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void CopyMeshToMeshCellData(const TInputMesh *in, TOutputMesh *out)
{
  typedef typename TInputMesh::CellDataContainer        InputCellDataContainer;
  typedef typename TOutputMesh::CellDataContainer       OutputCellDataContainer;
  typedef typename InputCellDataContainer::ConstPointer InputCellDataContainerConstPointer;
  typedef typename OutputCellDataContainer::Pointer     OutputCellDataContainerPointer;

  InputCellDataContainerConstPointer inputCellData = in->GetCellData();

  if ( inputCellData.IsNull() )
    {
    // There is nothing to copy
    return;
    }

  OutputCellDataContainerPointer outputCellData = OutputCellDataContainer::New();
  outputCellData->Reserve( inputCellData->Size() );

  // Copy point data
  typedef typename InputCellDataContainer::ConstIterator InputCellDataContainerConstIterator;
  InputCellDataContainerConstIterator inIt = inputCellData->Begin();
  while ( inIt != inputCellData->End() )
    {
    outputCellData->SetElement( inIt.Index(), inIt.Value() );
    inIt++;
    }

  out->SetCellData(outputCellData);
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void CopyMeshToMeshPointData(const TInputMesh *in, TOutputMesh *out)
{
  typedef typename TOutputMesh::PointDataContainer   OutputPointDataContainer;
  typedef typename OutputPointDataContainer::Pointer OutputPointDataContainerPointer;
  typedef typename TInputMesh::PointDataContainer    InputPointDataContainer;

  const InputPointDataContainer *inputPointData = in->GetPointData();

  if ( inputPointData == NULL )
    {
    // There is nothing to copy
    return;
    }

  OutputPointDataContainerPointer outputPointData = OutputPointDataContainer::New();
  outputPointData->Reserve( inputPointData->Size() );

  // Copy point data
  typedef typename InputPointDataContainer::ConstIterator InputPointDataContainerConstIterator;
  InputPointDataContainerConstIterator inIt = inputPointData->Begin();
  while ( inIt != inputPointData->End() )
    {
    outputPointData->SetElement( inIt.Index(), inIt.Value() );
    inIt++;
    }

  out->SetPointData(outputPointData);
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void CopyMeshToMeshCells(const TInputMesh *in, TOutputMesh *out)
{
  // Copy cells
  typedef typename TInputMesh::CellsContainer         InputCellsContainer;
  typedef typename InputCellsContainer::ConstPointer  InputCellsContainerConstPointer;
  typedef typename InputCellsContainer::ConstIterator InputCellsContainerConstIterator;
  typedef typename TInputMesh::PolygonCellType        InputPolygonCellType;
  typedef typename TInputMesh::PointIdList            InputPointIdList;
  typedef typename TInputMesh::CellTraits             InputCellTraits;
  typedef typename InputCellTraits::PointIdInternalIterator
  InputPointsIdInternalIterator;

  out->SetCellsAllocationMethod(TOutputMesh::CellsAllocatedDynamicallyCellByCell);

  InputCellsContainerConstPointer inCells = in->GetCells();

  if ( inCells )
    {
    InputCellsContainerConstIterator cIt = inCells->Begin();
    while ( cIt != inCells->End() )
      {
      InputPolygonCellType *pe = dynamic_cast< InputPolygonCellType * >( cIt.Value() );
      if ( pe )
        {
        InputPointIdList              points;
        InputPointsIdInternalIterator pit = pe->InternalPointIdsBegin();
        while ( pit != pe->InternalPointIdsEnd() )
          {
          points.push_back( ( *pit ) );
          ++pit;
          }
        out->AddFaceWithSecurePointList(points, false);
        }
      cIt++;
      }
    }
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void CopyMeshToMeshEdgeCells(const TInputMesh *in, TOutputMesh *out)
{
  // Copy Edge Cells
  typedef typename TInputMesh::CellsContainer         InputCellsContainer;
  typedef typename InputCellsContainer::ConstPointer  InputCellsContainerConstPointer;
  typedef typename InputCellsContainer::ConstIterator InputCellsContainerConstIterator;
  typedef typename TInputMesh::EdgeCellType           InputEdgeCellType;

  InputCellsContainerConstPointer inEdgeCells = in->GetEdgeCells();

  if ( inEdgeCells )
    {
    InputCellsContainerConstIterator ecIt = inEdgeCells->Begin();
    while ( ecIt != inEdgeCells->End() )
      {
      InputEdgeCellType *pe = dynamic_cast< InputEdgeCellType * >( ecIt.Value() );
      if ( pe )
        {
        out->AddEdgeWithSecurePointList( pe->GetQEGeom()->GetOrigin(),
                                         pe->GetQEGeom()->GetDestination() );
        }
      ecIt++;
      }
    }
}

// ---------------------------------------------------------------------
template< class TInputMesh, class TOutputMesh >
void CopyMeshToMeshPoints(const TInputMesh *in, TOutputMesh *out)
{
  // Copy points
  typedef typename TInputMesh::PointsContainer         InputPointsContainer;
  typedef typename InputPointsContainer::ConstPointer  InputPointsContainerConstPointer;
  typedef typename InputPointsContainer::ConstIterator InputPointsContainerConstIterator;
  typedef typename TOutputMesh::PointType              OutputPointType;

  InputPointsContainerConstPointer inPoints = in->GetPoints();

  if ( inPoints )
    {
    InputPointsContainerConstIterator inIt = inPoints->Begin();
    OutputPointType                   pOut;

    while ( inIt != inPoints->End() )
      {
      pOut.CastFrom( inIt.Value() );
      out->SetPoint(inIt.Index(), pOut);
      inIt++;
      }
    }
}
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.hxx"
#endif

#endif
