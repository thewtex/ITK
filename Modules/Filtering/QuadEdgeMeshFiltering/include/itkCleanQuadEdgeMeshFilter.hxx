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
#ifndef __itkCleanQuadEdgeMeshFilter_hxx
#define __itkCleanQuadEdgeMeshFilter_hxx

#include "itkCleanQuadEdgeMeshFilter.h"

namespace itk
{

template< class TInputMesh, class TOutputMesh >
CleanQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CleanQuadEdgeMeshFilter()
{
  this->m_AbsoluteTolerance  = NumericTraits< InputCoordRepType >::Zero;
  this->m_RelativeTolerance  = NumericTraits< InputCoordRepType >::Zero;
}


template< class TInputMesh, class TOutputMesh >
void
CleanQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::GenerateData()
{
  InputCoordRepType zeroValue = NumericTraits< InputCoordRepType >::Zero;

  InputCoordRepType absoluteToleranceSquared = this->m_AbsoluteTolerance * this->m_AbsoluteTolerance;
  if ( ( this->m_AbsoluteTolerance == zeroValue ) && ( this->m_RelativeTolerance != zeroValue ) )
    {
    itkAssertOrThrowMacro( ( m_RelativeTolerance > zeroValue ) && ( m_RelativeTolerance < 1. ),
                           "Relative tolerance out of range" );
    BoundingBoxPointer bounding_box = BoundingBoxType::New();
    bounding_box->SetPoints( this->GetInput()->GetPoints() );
    bounding_box->ComputeBoundingBox();

    absoluteToleranceSquared = this->m_RelativeTolerance * this->m_RelativeTolerance
                           * bounding_box->GetDiagonalLength2();
    }

  this->MergePoints( absoluteToleranceSquared );
  this->CleanPoints();
}


template< class TInputMesh, class TOutputMesh >
void
CleanQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::MergePoints( const InputCoordRepType absoluteToleranceSquared )
{
  OutputMeshPointer output = this->GetOutput();

  CriterionPointer criterion = CriterionType::New();

  criterion->SetTopologicalChange( false );
  criterion->SetMeasureBound( absoluteToleranceSquared );

  DecimationPointer decimate = DecimationType::New();
  decimate->SetInput( this->GetInput() );
  decimate->SetCriterion(criterion);
  decimate->Update();

  InputMeshPointer temp = decimate->GetOutput();

  InputPointsContainerIterator p_it = temp->GetPoints()->Begin();
  InputPointsContainerIterator p_end = temp->GetPoints()->End();

  OutputPointType pOut;

  while ( p_it != p_end )
    {
    pOut.CastFrom( p_it.Value() );
    output->SetPoint(p_it.Index(), pOut);
    ++p_it;
    }

  // Copy Edge Cells
  InputCellsContainerIterator c_it = temp->GetEdgeCells()->Begin();
  InputCellsContainerIterator c_end = temp->GetEdgeCells()->End();
  InputEdgeCellType *         qe;
  InputQEPrimal *             QEGeom;

  while ( c_it != c_end )
    {
    qe = dynamic_cast< InputEdgeCellType * >( c_it.Value() );
    QEGeom = qe->GetQEGeom();
    output->AddEdgeWithSecurePointList( QEGeom->GetOrigin(),
                                        QEGeom->GetDestination() );
    ++c_it;
    }

  // Copy cells
  c_it = temp->GetCells()->Begin();
  c_end = temp->GetCells()->End();
  InputPolygonCellType *pe;

  while ( c_it != c_end )
    {
    pe = dynamic_cast< InputPolygonCellType * >( c_it.Value() );
    if ( pe )
      {
      InputPointIdList points;

      for ( InputPointsIdInternalIterator pit = pe->InternalPointIdsBegin();
            pit != pe->InternalPointIdsEnd(); ++pit )
        {
        points.push_back( ( *pit ) );
        }
      output->AddFaceWithSecurePointList(points);
      }
    ++c_it;
    }
}


template< class TInputMesh, class TOutputMesh >
void
CleanQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::CleanPoints()
{
  OutputMeshPointer output = this->GetOutput();

  OutputPointsContainerIterator p_it = output->GetPoints()->Begin();
  OutputPointsContainerIterator p_end = output->GetPoints()->End();
  OutputPointIdentifier         id(0);

  while ( p_it != p_end )
    {
    id = p_it->Index();
    if ( output->FindEdge(id) == 0 )
      {
      output->DeletePoint(id);
      }
    ++p_it;
    }

  output->SqueezePointsIds();
}


template< class TInputMesh, class TOutputMesh >
void
CleanQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "AbsoluteTolerance: " << m_AbsoluteTolerance << std::endl;
  os << indent << "RelativeTolerance: " << m_RelativeTolerance << std::endl;
}

} // end namespace itk

#endif
