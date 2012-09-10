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
#ifndef __itkLaplaceBeltramiFilter_hxx
#define __itkLaplaceBeltramiFilter_hxx

#include "itkLaplaceBeltramiFilter.h"
#include "itkExceptionObject.h"
#include "itkCrossHelper.h"

#include "vnl/vnl_math.h"
#include "vnl/vnl_sparse_matrix.txx"
#include "VNLSparseSymmetricEigensystemTraits.h"

#include "itkArray.h"

namespace itk
{
template <class TInputMesh, class TOutputMesh, class TAdaptor >
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::LaplaceBeltramiFilter()
{
  SetEigenValueCount(0);
  SetHarmonicScaleValue(0.0);
  SetBoundaryConditionType(DIRICHLET);
}

/**
 *
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Harmonic " << this->m_Harmonics << std::endl;
  os << indent << "Eigen Value Count " << this->m_EigenValueCount << std::endl;
  os << indent << "Vertex Areas dimensions " << this->m_VertexAreas.rows() << " x "
     << this->m_VertexAreas.cols() << std::endl;
  os << indent << "Laplace Beltrami Operator dimensions "
     << this->m_LBOperator.rows() << " x "
     << this->m_LBOperator.cols() << std::endl;
  os << indent << "BoundaryConditionType "
     << this->m_BoundaryConditionType << std::endl;

}

/**
 * Determine the area associated with each vertex
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
bool
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::ComputeVertexAreas(std::map< InputPointIdentifier, double > &vertexAreas)
{
  InputMeshConstPointer  inputMesh      =  this->GetInput();

  const unsigned int sides = 3;

  std::map< InputCellIdentifier, double > faceAreas;

  InputCellsContainerConstPointer cells = inputMesh->GetCells();

  InputCellsContainerConstIterator cIt = cells->Begin();
  InputCellsContainerConstIterator cEnd = cells->End();

  // compute areas of faces and vertices
  while( cIt != cEnd )
    {
    InputCellIdentifier cellId = cIt->Index();
    const InputCellType* cellPtr = cIt->Value();

    unsigned int aCellNumberOfPoints = cellPtr->GetNumberOfPoints();
    if( aCellNumberOfPoints != sides )
      {
      itkExceptionMacro("cell has " << aCellNumberOfPoints << " points\n"
      "This filter can only process triangle meshes.");
      return false;
      }

    const unsigned long *tp = cellPtr->GetPointIds();

    InputPointType v1 = inputMesh->GetPoint( static_cast< InputPointIdentifier >( tp[0] ) );
    InputPointType v2 = inputMesh->GetPoint( static_cast< InputPointIdentifier >( tp[1] ) );
    InputPointType v3 = inputMesh->GetPoint( static_cast< InputPointIdentifier >( tp[2] ) );

    // determine if face is obtuse
    typename InputPointType::VectorType x12 = v2 - v1;
    typename InputPointType::VectorType x13 = v3 - v1;
    typename InputPointType::VectorType x23 = v3 - v2;

    double norm12 = x12.GetNorm();
    double norm13 = x13.GetNorm();
    double norm23 = x23.GetNorm();

    double c1 = x12 * x13;
    c1 /= norm12 * norm13;

    double c2 =  - x12 * x23;
    c2 /= norm12 * norm23;

    double c3 = x13 * x23;
    c3 /= norm13 * norm23;

    CrossProductType cross;
    faceAreas[ cellId ] = 0.5 * cross( x12, x13 ).GetNorm();

    if( vertexAreas.find( tp[0] ) == vertexAreas.end() )
      {
      vertexAreas[ tp[0] ] = 0.;
      }
    if( vertexAreas.find( tp[1] ) == vertexAreas.end() )
      {
      vertexAreas[ tp[1] ] = 0.;
      }
    if( vertexAreas.find( tp[2] ) == vertexAreas.end() )
      {
      vertexAreas[ tp[2] ] = 0.;
      }

    if (c1 < 0.0)  // if the projection is negative, angle 1 is obtuse
      {
      vertexAreas[ tp[0] ] += faceAreas[cellId] * 0.5;
      vertexAreas[ tp[1] ] += faceAreas[cellId] * 0.25;
      vertexAreas[ tp[2] ] += faceAreas[cellId] * 0.25;
      }
    else if (c2 < 0.0)  // if the projection is negative, angle 2 is obtuse
      {
      vertexAreas[ tp[0] ] += faceAreas[cellId] * 0.25;
      vertexAreas[ tp[1] ] += faceAreas[cellId] * 0.5;
      vertexAreas[ tp[2] ] += faceAreas[cellId] * 0.25;
      }
    else if (c3 < 0.0)  // if the projection is negative, angle 3 is obtuse
      {
      vertexAreas[ tp[0] ] += faceAreas[cellId] * 0.25;
      vertexAreas[ tp[1] ] += faceAreas[cellId] * 0.25;
      vertexAreas[ tp[2] ] += faceAreas[cellId] * 0.5;
      }
    else  // no obtuse angles
      {
      double cot1 = c1 / vcl_sqrt(1.0 - c1 * c1);
      double cot2 = c2 / vcl_sqrt(1.0 - c2 * c2);
      double cot3 = c3 / vcl_sqrt(1.0 - c3 * c3);

      vertexAreas[ tp[0] ] += ( cot3 * ( x12 * x12 ) + cot2 * ( x13 * x13 ) ) * 0.125;
      vertexAreas[ tp[1] ] += ( cot3 * ( x12 * x12 ) + cot1 * ( x23 * x23 ) ) * 0.125;
      vertexAreas[ tp[2] ] += ( cot2 * ( x13 * x13 ) + cot1 * ( x23 * x23 ) ) * 0.125;
      }
    ++cIt;
    }

  return true;

}

/**
 * Determine edges, edge angles, and boundary vertices
 * return the edgeCount
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
unsigned int
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::GetEdges(itk::Array2D<unsigned int> &edges,
           itk::Array<unsigned int> &boundaryVertex,
           itk::Array2D<double> &edgeAngles)
{
  InputMeshConstPointer  inputMesh      =  this->GetInput();

  unsigned int cellCount = inputMesh->GetNumberOfCells();
  unsigned int vertexCount = inputMesh->GetNumberOfPoints();

  // compute edges and detect boundary
  vnl_sparse_matrix<unsigned int> edgeMatrix(vertexCount, vertexCount);
  edges.set_size(cellCount * 3, 2);
  edges.Fill(0);
  unsigned int edgeCount = 0;

  InputCellAutoPointer cellPtr;

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    const unsigned long *tp;
    tp = cellPtr->GetPointIds();

    if (edgeMatrix(tp[0], tp[1]) == 0)
      {
      edgeMatrix(tp[0], tp[1]) = edgeCount + 1;
      edgeMatrix(tp[1], tp[0]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[0];
      edges(edgeCount, 1) = tp[1];
      edgeCount++;
      }
    if (edgeMatrix(tp[1], tp[2]) == 0)
      {
      edgeMatrix(tp[1], tp[2]) = edgeCount + 1;
      edgeMatrix(tp[2], tp[1]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[1];
      edges(edgeCount, 1) = tp[2];
      edgeCount++;
      }
    if (edgeMatrix(tp[0], tp[2]) == 0)
      {
      edgeMatrix(tp[2], tp[0]) = edgeCount + 1;
      edgeMatrix(tp[0], tp[2]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[2];
      edges(edgeCount, 1) = tp[0];
      edgeCount++;
      }
    }

  edges = edges.extract(edgeCount, 2);
  typedef vnl_sparse_matrix<unsigned int> EdgeFaceMatrixType;
  EdgeFaceMatrixType edgeFace(edgeCount, cellCount);

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    const unsigned long *tp;
    tp = cellPtr->GetPointIds();
    edgeFace(edgeMatrix(tp[0], tp[1]) - 1, cellId) = 1;
    edgeFace(edgeMatrix(tp[1], tp[2]) - 1, cellId) = 1;
    edgeFace(edgeMatrix(tp[2], tp[0]) - 1, cellId) = 1;
    }

  boundaryVertex.set_size(vertexCount);
  boundaryVertex.Fill(0);

  edgeAngles.set_size(edgeCount, 2);
  edgeAngles.Fill(0.0);

  for (unsigned int edgeIx = 0; edgeIx < edgeCount; edgeIx++)
    {
    EdgeFaceMatrixType::row aRow = edgeFace.get_row(edgeIx);
    for (unsigned int u = 0; u < aRow.size(); u++)
      {
      int faceIx = aRow[u].first;

      // get vertices of the face
      inputMesh->GetCell( faceIx, cellPtr );
      const unsigned long *tp;
      tp = cellPtr->GetPointIds();

      long i1 = -1;
      long i2 = -1;

      for (unsigned int vIx = 0; vIx < 3; vIx++)
        {
        if (tp[vIx] == edges(edgeIx, 0))
          {
          i1 = vIx;
          }
        if (tp[vIx] == edges(edgeIx, 1))
          {
          i2 = vIx;
          }
        }

      if( i1 < 0 || i2 < 0 )
        {
        itkExceptionMacro("failed to find edges for cell " << faceIx << "\n");
        return 0;
        }

      unsigned int s = i1 + i2;
      unsigned int i3;
      if (s == 1)
        {
        i3 = 2;
        }
      else if (s == 2)
        {
        i3 = 1;
        }
      else if (s == 3)
        {
        i3 = 0;
        }
      else
        {
        itkExceptionMacro("bad vertex indices " << i1 << ", " << i2 << " for face " << faceIx << "\n");
        return 0;
        }

      InputPointType verts[3];
      inputMesh->GetPoint((int)(tp[0]), &verts[0]);
      inputMesh->GetPoint((int)(tp[1]), &verts[1]);
      inputMesh->GetPoint((int)(tp[2]), &verts[2]);

      typename InputPointType::VectorType x1 = verts[ i1 ] - verts[ i3 ];
      typename InputPointType::VectorType x2 = verts[ i2 ] - verts[ i3 ];

      CrossProductType cross;
      typename InputPointType::VectorType v21Xv32 = cross( (verts[1] - verts[0]), (verts[2] - verts[0]) );
      typename InputPointType::VectorType x1Xx2 = cross( x1, x2 );

      double a = x1Xx2 * v21Xv32;
      double b = x1 * x2;

      edgeAngles(edgeIx, u) = b / vcl_sqrt( vnl_math_abs( a ) );
      }

    if (aRow.size() == 1)
      {
      // have a boundary edge
      boundaryVertex(edges(edgeIx, 0)) = 1;
      boundaryVertex(edges(edgeIx, 1)) = 1;
      edgeAngles(edgeIx, 1) = 0;
      }
    }
  return edgeCount;
}

/**
 * compute the Laplacian matrix
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::ComputeLaplacian(itk::Array2D<unsigned int> &edges,
                   itk::Array2D<double> &edgeAngles,
                   std::map< InputPointIdentifier, double > &vertexAreas)
{
  unsigned int vertexCount = this->GetInput()->GetNumberOfPoints();

  m_LBOperator.set_size(vertexCount, vertexCount);

  for (unsigned int edgeIx = 0; edgeIx < edges.rows(); edgeIx++)
    {
    double laplacian =
        (edgeAngles(edgeIx, 0) + edgeAngles(edgeIx, 1)) * 0.5;
    m_LBOperator(edges(edgeIx, 0), edges(edgeIx, 1)) = laplacian;
    m_LBOperator(edges(edgeIx, 1), edges(edgeIx, 0)) = laplacian;
    }

  m_VertexAreas.set_size(vertexCount, vertexCount);
  for (unsigned int vertexIx = 0; vertexIx < vertexCount; vertexIx++)
    {
    m_VertexAreas(vertexIx, vertexIx) = vertexAreas[ vertexIx ];
    double accum = 0.0;
    typename LBMatrixType::row aRow = m_LBOperator.get_row(vertexIx);
    for (unsigned int u = 0; u < aRow.size(); u++)
      {
      accum += aRow[u].second;
      }
    m_LBOperator(vertexIx, vertexIx) = -accum;
    }
}

/**
 * compute the surface harmonics
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
bool
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::ComputeHarmonics(itk::Array<unsigned int> &boundaryVertex)
{
  unsigned int vertexCount = this->GetInput()->GetNumberOfPoints();

  // Need to solve the following eigensystem:
  // A * V = B * V * D, or
  // m_LBOperator * eVectors = m_VertexAreas * eVectors * eValues

  int elementCount = vertexCount;

  MatrixType A;
  MatrixType B;

  if (!boundaryVertex.sum())
    {
    // closed surface
    A = m_LBOperator;
    B = m_VertexAreas;
    }
  else if (m_BoundaryConditionType == VONNEUMAN)
    {
    // for Von Neuman, zero out the "area" of the boundary vertices
    // before solving standard eigenvalue problem
    A = m_LBOperator;
    B = AdaptorType::InitializeSparseMatrix(m_VertexAreas.rows());

    elementCount = 0;
    // Zero out diagonal matrix for boundary values
    for (unsigned int ix = 0; ix < vertexCount; ix++)
      {
      if (!boundaryVertex(ix))
        {
        AdaptorType::FillMatrix(B, ix, ix, m_VertexAreas(ix, ix));
        elementCount++;
        }
      }
    }
  else if (m_BoundaryConditionType == DIRICHLET)
    {
    // non-boundary point count
    elementCount -= boundaryVertex.sum();
    A = AdaptorType::InitializeSparseMatrix(elementCount);
    B = AdaptorType::InitializeSparseMatrix(elementCount);

    int colCount = 0;
    int rowCount = 0;
    // collapse matrix by removing boundary values
    for (unsigned int ix = 0; ix < vertexCount; ix++)
      {
      colCount = 0;
      if (!boundaryVertex(ix))
        {
        AdaptorType::FillMatrix(B, rowCount, rowCount, m_VertexAreas(ix, ix));
        for (unsigned int jx = 0; jx < vertexCount; jx++)
          {
          if (!boundaryVertex(jx))
            {
            if (m_LBOperator(ix, jx))
              {
              AdaptorType::FillMatrix(A, rowCount, colCount, m_LBOperator(ix, jx));
              }
            colCount++;
            }
          }
        rowCount++;
        }
      }
    }
  else
    {
    itkExceptionMacro("Unknown Boundary Condition Type:  " << m_BoundaryConditionType << "\n");
    return false;
    }

  EigenvectorMatrixType eigenvectors;
  VectorType            eigenvalues;
  if (!AdaptorType::Calculate(A, B, this->m_EigenValueCount,
                          eigenvectors, eigenvalues,
                          false, true, 1.0e-10, 10000, 1.0))
    {
    // failed to compute eigenvalues
    itkExceptionMacro("Eigensystem failure\n");
    return false;
    }

  // get the output
  this->m_Eigenvalues = eigenvalues;
  this->m_Harmonics.set_size(this->m_EigenValueCount, vertexCount);
  for (unsigned int evIx = 0; evIx < this->m_EigenValueCount; evIx++)
    {
    vnl_vector< double > sseEigenvector = eigenvectors.get_column(evIx);
    vnl_vector< double > eigenvector(vertexCount);
    eigenvector = 0.0;

    int nonboundaryCount = 0;
    for (unsigned int vxIx = 0; vxIx < vertexCount; vxIx++)
      {
      if (!boundaryVertex(vxIx) ||
          (m_BoundaryConditionType != DIRICHLET))
        {
        eigenvector(vxIx) = sseEigenvector(nonboundaryCount++);
        }
      }
    this->m_Harmonics.set_row(evIx, eigenvector);
    }
    return true;
}

/**
 * This method causes the filter to generate its output.
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::GenerateData(void)
{
  this->CopyInputMeshToOutputMesh();

  InputMeshConstPointer  inputMesh      =  this->GetInput();
  OutputMeshPointer      outputMesh     =  this->GetOutput();

  std::map< InputPointIdentifier, double > vertexAreas;

  // compute areas of faces and vertices
  if (!ComputeVertexAreas(vertexAreas))
    {
    return;
    }

  const double FaceAreaTolerance = 1.0e-15;
  typename std::map< InputPointIdentifier, double >::const_iterator vIt = vertexAreas.begin();
  typename std::map< InputPointIdentifier, double >::const_iterator vEnd = vertexAreas.end();

  while( vIt != vEnd )
    {
    if( vIt->second < FaceAreaTolerance )
      {
        itkExceptionMacro("Vertex " << vIt->first << " has virtually no face area:  "
          << vIt->second << "\n");
        return;
      }
    ++vIt;
    }

  // compute edges and detect boundary
  itk::Array2D<unsigned int> edges;
  itk::Array<unsigned int> boundaryVertex;
  itk::Array2D<double> edgeAngles;
  if (!GetEdges(edges, boundaryVertex, edgeAngles))
    return;

  // compute the Laplacian matrix
  ComputeLaplacian(edges, edgeAngles, vertexAreas);

  // compute harmonics?
  if (this->m_EigenValueCount)
    {
    ComputeHarmonics(boundaryVertex);
    this->SetSurfaceHarmonic(0);
    }
  else
    {
    this->m_Harmonics.clear();
    }
}

/**
 * Get a single surface harmonic
 */
template <class TInputMesh, class TOutputMesh, class TAdaptor >
bool
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TAdaptor >
::SetSurfaceHarmonic( unsigned int harmonic )
{
  if ( harmonic >= this->m_Harmonics.rows() )
    {
    itkExceptionMacro("Requested harmonic " << harmonic <<
        " outside range of available harmonics calculated (0 - " <<
        this->m_Harmonics.rows() << ").\n");
      return false;
    }

  OutputMeshPointer surface = this->GetOutput();

  vnl_vector<double> thisHarm = m_Harmonics.get_row(harmonic);
  if (this->m_HarmonicScaleValue)
    {
    // find max abs value of this harmonic
    double maxAbs = thisHarm.inf_norm();
    thisHarm *= ((this->m_HarmonicScaleValue - 1.0) * 0.5 )/ maxAbs;
    thisHarm += this->m_HarmonicScaleValue * 0.5;
    }

  for (unsigned int k = 0; k < this->m_Harmonics.cols(); k++)
    {
    surface->SetPointData(k, thisHarm(k));
    }

  this->Modified();

  return true;
}

} // end namespace itk

#endif
