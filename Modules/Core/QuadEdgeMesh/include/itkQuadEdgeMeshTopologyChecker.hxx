/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkQuadEdgeMeshTopologyChecker_hxx
#define itkQuadEdgeMeshTopologyChecker_hxx


namespace itk
{
template <typename TMesh>
QuadEdgeMeshTopologyChecker<TMesh>::QuadEdgeMeshTopologyChecker()
  : m_Mesh(nullptr)
  , m_ExpectedNumberOfPoints(PointIdentifier{})
  , m_ExpectedNumberOfEdges(CellIdentifier{})
  , m_ExpectedNumberOfFaces(CellIdentifier{})
  , m_ExpectedNumberOfBoundaries(CellIdentifier{})
  , m_ExpectedGenus(OffsetValueType{})
{}

template <typename TMesh>
bool
QuadEdgeMeshTopologyChecker<TMesh>::ValidateEulerCharacteristic() const
{
  if (!this->m_Mesh)
  {
    return (false);
  }

  auto boundaryEdges = BoundaryEdges::New();

  // Number of USED points
  const PointIdentifier numPoints = m_Mesh->ComputeNumberOfPoints();
  // Number of USED edges
  const CellIdentifier numEdges = m_Mesh->ComputeNumberOfEdges();
  // Number of USED faces
  const CellIdentifier numFaces = m_Mesh->ComputeNumberOfFaces();
  // Number of Boundaries
  typename BoundaryEdges::OutputType listOfBoundaries = boundaryEdges->Evaluate((*m_Mesh));
  auto                               numBounds = static_cast<CellIdentifier>(listOfBoundaries->size());
  delete listOfBoundaries;

  /**
   * Number of points
   *
   * There are two methods to get the number of points.
   * 1. itk::QuadEdgeMesh::ComputeNumberOfPoints()
   * 2. itk::Mesh::GetNumberOfPoints()
   *
   * As an itk::QuadEdgeMesh is an itk::Mesh by inheritance, the user
   * can use both. 1. will returned the number of points actually
   * used by at least one edge, while 2. will give you the number
   * of points in the container. Number of unused points can be found
   * by making the difference between the two values.
   */
  if (m_Mesh->GetNumberOfPoints() != numPoints)
  {
    // They are isolated vertices:
    return (false);
  }

  // The euler formula states:
  // numFaces - numEdges + numPoints == 2 - 2 * genus - numBounds
  // hence ( 2 - numBounds - numFaces + numEdges - numPoints ) must
  // be an odd number. Let's check it out:
  // Note that genus can take a negative value...
  const OffsetValueType twiceGenus = static_cast<OffsetValueType>(2) - OffsetValueType(numBounds) -
                                     OffsetValueType(numFaces) + OffsetValueType(numEdges) - OffsetValueType(numPoints);

  if (twiceGenus % 2)
  {
    return (false);
  }

  // Look is they are isolated edges
  CellsContainerConstIterator       cellIterator = m_Mesh->GetCells()->Begin();
  const CellsContainerConstIterator cellEnd = m_Mesh->GetCells()->End();
  while (cellIterator != cellEnd)
  {
    // Is the cell an Edge ?
    if (auto * cell = dynamic_cast<EdgeCellType *>(cellIterator.Value()))
    {
      if (QEPrimal * edge = cell->GetQEGeom())
      {
        // Is the edge without associated faces ?.
        if (edge->IsWire())
        {
          // Is it an isolated edge ?
          if (edge->IsIsolated() && edge->GetSym()->IsIsolated())
          {
            return (false);
          }
        }
      }
      else // cell->GetQEGeom( ) == nullptr
      {
        // supposedly impossible, throw exception
      }
    }

    ++cellIterator;
  } // end of while

  return (true);
}

template <typename TMesh>
void
QuadEdgeMeshTopologyChecker<TMesh>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ExpectedNumberOfPoints: " << m_ExpectedNumberOfPoints << std::endl;
  os << indent << "ExpectedNumberOfEdges: " << m_ExpectedNumberOfEdges << std::endl;
  os << indent << "ExpectedNumberOfFaces: " << m_ExpectedNumberOfFaces << std::endl;
  os << indent << "ExpectedNumberOfBoundaries: " << m_ExpectedNumberOfBoundaries << std::endl;
  os << indent << "ExpectedGenus: " << m_ExpectedGenus << std::endl;
  os << indent << "Mesh: " << m_Mesh << std::endl;
}
} // namespace itk
#endif
