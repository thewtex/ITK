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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints_hxx
#define __itkLaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints_hxx

#include "itkLaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints.h"

namespace itk
{
// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints(): Superclass(), m_Lambda(1), m_LambdaSquare(1)
{}
// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::ComputeVertexIdMapping()
{
  OutputMeshPointer output   = this->GetOutput();

  typename Superclass::OutputPointsContainerPointer points = output->GetPoints();

  OutputPointsContainerIterator pIt = points->Begin();
  OutputPointsContainerIterator pEnd = points->End();

  OutputPointIdentifier k = 0;

  while ( pIt != pEnd )
    {
    this->m_InternalMap.insert( typename OutputMapPointIdentifier::value_type(pIt->Index(), k++) );
    ++pIt;
    }
}
// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::SetLocalLambda( OutputPointIdentifier id, OutputCoordRepType iL )
{
  m_LocalLambdaSquare[ id ] = iL * iL;
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::FillMatrix(MatrixType & iM, VectorType & iBx, VectorType & iBy, VectorType & iBz)
{
  OutputMeshPointer output = this->GetOutput();

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
    RowIterator rEnd = row.end();

    while( rIt != rEnd )
      {
      id2 = rIt->first;
      weight = rIt->second;

      OutputPointType p = output->GetPoint( id2 );
      iBx[internalId1] += weight * p[0];
      iBy[internalId1] += weight * p[1];
      iBz[internalId1] += weight * p[2];

      internalId2 = static_cast< unsigned int >( this->m_InternalMap[id2] );
      SolverTraits::FillMatrix(iM, internalId1, internalId2, weight);

      ++rIt;
      }
    ++it;
    }
}


// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::GenerateData()
{
  this->m_LambdaSquare = this->m_Lambda * this->m_Lambda;

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

    MatrixType Mt( M.transpose() );
    // SolverTraits::TransposeMatrix( M, Mt );

    MatrixType A( Mt * M );
    // SolverTraits::MultiplyMatrices( Mt, M, A );

    VectorType Cx, Cy, Cz;
    Mt.mult( Bx, Cx );
    Mt.mult( By, Cy );
    Mt.mult( Bz, Cz );

//    SolverTraits::MultiplyMatrixVector( Mt, Bx, Cx );
//    SolverTraits::MultiplyMatrixVector( Mt, By, Cy );
//    SolverTraits::MultiplyMatrixVector( Mt, Bz, Cz );

    OutputPointsContainerPointer points = output->GetPoints();

    for ( ConstraintMapConstIterator cIt = this->m_Constraints.begin();
          cIt != this->m_Constraints.end();
          ++cIt )
      {
      OutputPointIdentifier id = cIt->first;
      OutputPointType p = points->GetElement(id);
      p += cIt->second;

      unsigned int t = this->m_InternalMap[ id ];

      OutputCoordRepType l2 = m_LambdaSquare;

      typename std::map< OutputPointIdentifier, OutputCoordRepType >::iterator lambdaIt = this->m_LocalLambdaSquare.find( id );
      if( lambdaIt != this->m_LocalLambdaSquare.end() )
        {
        l2 = lambdaIt->second;
        }

      Cx[ t ] += l2 * p[0];
      Cy[ t ] += l2 * p[1];
      Cz[ t ] += l2 * p[2];

      SolverTraits::AddToMatrix( A, this->m_InternalMap[ id ], this->m_InternalMap[ id ], l2 );
      }

    VectorType X = SolverTraits::InitializeVector(N);
    X.fill(0.);

    VectorType Y = SolverTraits::InitializeVector(N);
    Y.fill(0.);

    VectorType Z = SolverTraits::InitializeVector(N);
    Z.fill(0.);

    this->SolveLinearSystems(A, Cx, Cy, Cz, X, Y, Z);

    OutputPointIdentifier id;

    OutputMapPointIdentifierIterator it = this->m_InternalMap.begin();
    while ( it != this->m_InternalMap.end() )
      {
      id = it->first;
      unsigned int internalId = static_cast< unsigned int >( it->second );

      OutputPointType& p = points->ElementAt(id);

      OutputCoordRepType x = static_cast< OutputCoordRepType >( X[internalId] );
      p[0] = x;

      OutputCoordRepType y = static_cast< OutputCoordRepType >( Y[internalId] );
      p[1] = y;

      OutputCoordRepType z = static_cast< OutputCoordRepType >( Z[internalId] );
      p[2] = z;

      ++it;
      }
    }
}

// ---------------------------------------------------------------------
template< typename TInputMesh, typename TOutputMesh, typename TSolverTraits >
void
LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< TInputMesh, TOutputMesh, TSolverTraits >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk

#endif
