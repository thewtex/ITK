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
#ifndef itkHexahedronCell_h
#define itkHexahedronCell_h

#include "itkQuadrilateralCell.h"
#include "itkHexahedronCellTopology.h"
#include "itkMakeFilled.h"

#include <array>

namespace itk
{
/** \class HexahedronCell
 *  \brief Represents a hexahedron (cuboid) for a Mesh.
 *
 * HexahedronCell represents a hexahedron, more precisely, a cuboid, for a Mesh.
 *
 * \todo When reviewing this class, the documentation of the  template
 * parameters MUST be fixed.
 *
 * NOTE: ONLY 3D implementations are instrumented.  All other dimensions
 *       result in incorrect processing.
 *
 * \ingroup MeshObjects
 * \ingroup ITKCommon
 */

template <typename TCellInterface>
class ITK_TEMPLATE_EXPORT HexahedronCell
  : public TCellInterface
  , private HexahedronCellTopology
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(HexahedronCell);

  /** Standard class type aliases. */
  /** @ITKStartGrouping */
  itkCellCommonTypedefs(HexahedronCell);
  itkCellInheritedTypedefs(TCellInterface);
  /** @ITKEndGrouping */
  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(HexahedronCell);

  /** The type of boundary for this triangle's vertices. */
  using VertexType = VertexCell<TCellInterface>;
  using VertexAutoPointer = typename VertexType::SelfAutoPointer;

  /** The type of boundary for this triangle's edges. */
  using EdgeType = LineCell<TCellInterface>;
  using EdgeAutoPointer = typename EdgeType::SelfAutoPointer;

  /** The type of boundary for this hexahedron's faces. */
  using FaceType = QuadrilateralCell<TCellInterface>;
  using FaceAutoPointer = typename FaceType::SelfAutoPointer;

  /** Hexahedron-specific topology numbers. */
  static constexpr unsigned int NumberOfPoints = 8;
  static constexpr unsigned int NumberOfVertices = 8;
  static constexpr unsigned int NumberOfEdges = 12;
  static constexpr unsigned int NumberOfFaces = 6;
  static constexpr unsigned int CellDimension = 3;

  /** HARDCODE Implementation requirements, while
   * allowing general interface.  The General interface
   * is needed to facilitate the general SpatialObject
   * loader.
   */
  static constexpr unsigned int CellDimension3D = 3;
  static constexpr unsigned int PointDimension3D = 3;

  /** Implement the standard CellInterface. */
  /** @ITKStartGrouping */
  [[nodiscard]] CellGeometryEnum
  GetType() const override
  {
    return CellGeometryEnum::HEXAHEDRON_CELL;
  }
  void
  MakeCopy(CellAutoPointer &) const override;
  /** @ITKEndGrouping */
  [[nodiscard]] unsigned int
  GetDimension() const override;

  [[nodiscard]] unsigned int
  GetNumberOfPoints() const override;

  CellFeatureCount
  GetNumberOfBoundaryFeatures(int dimension) const override;

  bool
  GetBoundaryFeature(int dimension, CellFeatureIdentifier, CellAutoPointer &) override;
  void
  SetPointIds(PointIdConstIterator first) override;

  void
  SetPointIds(PointIdConstIterator first, PointIdConstIterator last) override;

  void
  SetPointId(int localId, PointIdentifier) override;
  PointIdIterator
  PointIdsBegin() override;

  PointIdConstIterator
  PointIdsBegin() const override;

  PointIdIterator
  PointIdsEnd() override;

  PointIdConstIterator
  PointIdsEnd() const override;

  /** Hexahedron-specific interface. */
  virtual CellFeatureCount
  GetNumberOfVertices() const;

  virtual CellFeatureCount
  GetNumberOfEdges() const;

  virtual CellFeatureCount
  GetNumberOfFaces() const;

  virtual bool
  GetVertex(CellFeatureIdentifier, VertexAutoPointer &);
  virtual bool
  GetEdge(CellFeatureIdentifier, EdgeAutoPointer &);
  virtual bool
  GetFace(CellFeatureIdentifier, FaceAutoPointer &);

  /** Evaluate the position inside the cell */
  bool
  EvaluatePosition(CoordinateType *,
                   PointsContainer *,
                   CoordinateType *,
                   CoordinateType[],
                   double *,
                   InterpolationWeightType *) override;

  /** Visitor interface */
  itkCellVisitMacro(CellGeometryEnum::HEXAHEDRON_CELL);

protected:
  /** Store the number of points needed for a hexahedron. */
  std::array<PointIdentifier, NumberOfPoints> m_PointIds{ MakeFilled<std::array<PointIdentifier, NumberOfPoints>>(
    NumericTraits<PointIdentifier>::max()) };

  void
  InterpolationDerivs(CoordinateType pcoords[Self::CellDimension],
                      CoordinateType derivs[Self::CellDimension * Self::NumberOfPoints]);
  void
  InterpolationFunctions(CoordinateType pcoords[Self::CellDimension], InterpolationWeightType sf[Self::NumberOfPoints]);
  void
  EvaluateLocation(int &                     itkNotUsed(subId),
                   PointsContainer *         points,
                   CoordinateType            pcoords[Self::CellDimension],
                   CoordinateType            x[Self::CellDimension],
                   InterpolationWeightType * weights);

public:
  HexahedronCell() = default;

  ~HexahedronCell() override = default;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkHexahedronCell.hxx"
#endif

#endif
