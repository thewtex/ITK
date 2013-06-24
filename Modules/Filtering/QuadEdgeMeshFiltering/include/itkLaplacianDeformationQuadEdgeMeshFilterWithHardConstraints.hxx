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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints_hxx
#define __itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints_hxx

#include "itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints.h"

namespace itk
{
// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints():Superclass()
{}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::ComputeVertexIdMapping()
{
  OutputMeshPointer output   = this->GetOutput();

  typename Superclass::OutputPointsContainerPointer points = output->GetPoints();

  OutputPointsContainerIterator pIt = points->Begin();
  OutputPointsContainerIterator pEnd = points->End();

  OutputPointIdentifier k = 0;
  OutputPointIdentifier id = 0;

  while ( pIt != pEnd )
    {
    id = pIt->Index();

    if ( this->m_Constraints.find(id) == this->m_Constraints.end() )
      {
      this->m_InternalMap.insert( typename OutputMapPointIdentifier::value_type(id, k++) );
      }
    ++pIt;
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::FillMatrix(MatrixType & iM, VectorType & iBx, VectorType & iBy, VectorType & iBz)
{
  OutputMapPointIdentifierIterator it = this->m_InternalMap.begin();

  OutputPointIdentifier id1, id2;
  unsigned int internalId1, internalId2;
  OutputCoordRepType    weight;

  while ( it != this->m_InternalMap.end() )
    {
    id1 = it->first;
    internalId1 = static_cast< unsigned int >( it->second );

    std::map< OutputPointIdentifier, OutputCoordRepType > row;
    this->FillMatrixRow(id1, this->m_Order, NumericTraits< OutputCoordRepType >::One, row);

    typedef typename std::map< OutputPointIdentifier, OutputCoordRepType >::iterator RowIterator;

    RowIterator rIt = row.begin();

    while( rIt != row.end() )
      {
      id2 = rIt->first;
      weight = rIt->second;

      ConstraintMapConstIterator cIt = this->m_Constraints.find(id2);
      if ( cIt != this->m_Constraints.end() )
        {
        iBx[internalId1] -= weight * ( cIt->second )[0];
        iBy[internalId1] -= weight * ( cIt->second )[1];
        iBz[internalId1] -= weight * ( cIt->second )[2];
        }
      else
        {
        internalId2 = static_cast< unsigned int >( this->m_InternalMap[id2] );
        SolverTraits::AddToMatrix(iM, internalId1, internalId2, weight);
        }
      ++rIt;
      }
    ++it;
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::GenerateData()
{
  this->CopyInputMeshToOutputMesh();

  if ( !this->m_Constraints.empty() )
    {
    OutputMeshPointer output = this->GetOutput();

    this->m_CoefficientMap.clear();
    this->m_MixedAreaMap.clear();

    this->ComputeVertexIdMapping();

    unsigned int N = static_cast< unsigned int >( this->m_InternalMap.size() );

    MatrixType M = SolverTraits::InitializeSparseMatrix(N, N);

    VectorType Bx = SolverTraits::InitializeVector(N);
    Bx.fill(0.);

    VectorType By = SolverTraits::InitializeVector(N);
    By.fill(0.);

    VectorType Bz = SolverTraits::InitializeVector(N);
    Bz.fill(0.);

    this->FillMatrix(M, Bx, By, Bz);

    VectorType X = SolverTraits::InitializeVector(N);
    X.fill(0.);

    VectorType Y = SolverTraits::InitializeVector(N);
    Y.fill(0.);

    VectorType Z = SolverTraits::InitializeVector(N);
    Z.fill(0.);

    this->SolveLinearSystems(M, Bx, By, Bz, X, Y, Z);

    OutputPointIdentifier id;

    typename Superclass::OutputPointsContainerPointer points = output->GetPoints();

    OutputMapPointIdentifierIterator it = this->m_InternalMap.begin();
    while ( it != this->m_InternalMap.end() )
      {
      id = it->first;
      unsigned int internalId = static_cast< unsigned int >( it->second );

      OutputPointType& p = points->ElementAt(id);

      OutputCoordRepType dx = static_cast< OutputCoordRepType >( X[internalId] );
      p[0] += dx;

      OutputCoordRepType dy = static_cast< OutputCoordRepType >( Y[internalId] );
      p[1] += dy;

      OutputCoordRepType dz = static_cast< OutputCoordRepType >( Z[internalId] );
      p[2] += dz;

      ++it;
      }

    ConstraintMapConstIterator cIt  = this->m_Constraints.begin();
    ConstraintMapConstIterator cEnd = this->m_Constraints.end();

    while( cIt != cEnd )
      {
      id = cIt->first;
      OutputPointType& p = points->ElementAt(id);
      p += cIt->second;
      ++cIt;
      }
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk

#endif
