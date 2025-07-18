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
#ifndef itkDeformableSimplexMesh3DGradientConstraintForceFilter_h
#define itkDeformableSimplexMesh3DGradientConstraintForceFilter_h

#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkMesh.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkCovariantVector.h"

#include <set>
#include <vector>
#include "ITKDeformableMeshExport.h"

namespace itk
{
class ImageVoxel
{
public:
  // voxel coordinates
  unsigned int m_Vpos[3];
  // subvoxel coordinates (in cartesian space)
  double m_Spos[3];
  // voxel value converted to a double
  double m_Value;
  // distance from line origin
  double m_Distance;
  // index
  unsigned int m_Index;

  ImageVoxel() = default;
  ImageVoxel(const int * pos, const double * subpos, double val, double dist, unsigned int ind)
    : m_Value(val)
    , m_Distance(dist)
    , m_Index(ind)
  {
    this->m_Vpos[0] = pos[0];
    this->m_Vpos[1] = pos[1];
    this->m_Vpos[2] = pos[2];
    this->m_Spos[0] = subpos[0];
    this->m_Spos[1] = subpos[1];
    this->m_Spos[2] = subpos[2];
  }

  /// returns voxel X coordinate (voxel column)
  [[nodiscard]] unsigned int
  GetX() const
  {
    return m_Vpos[0];
  }
  /// returns voxel Y coordinate (voxel row)
  [[nodiscard]] unsigned int
  GetY() const
  {
    return m_Vpos[1];
  }
  /// returns voxel Z coordinate (voxel plane)
  [[nodiscard]] unsigned int
  GetZ() const
  {
    return m_Vpos[2];
  }
  /// returns voxel distance to origin
  [[nodiscard]] double
  GetDistance() const
  {
    return m_Distance;
  }
  /// returns voxel value
  [[nodiscard]] double
  GetValue() const
  {
    return m_Value;
  }
  /// returns voxel position

  /// set the value of the voxel
  void
  SetValue(const double val)
  {
    m_Value = val;
  }

  friend inline std::ostream &
  operator<<(std::ostream & os, const ImageVoxel & val)
  {
    os << "Vpos: " << val.m_Vpos << std::endl;
    os << "Spos: " << val.m_Spos << std::endl;
    os << "Value: " << val.m_Value << std::endl;
    os << "Distance: " << val.m_Distance << std::endl;
    os << "Index: " << val.m_Index << std::endl;

    return os;
  }
};
/** \class DeformableSimplexMesh3DGradientConstraintForceFilterEnums
 * \brief Contains all enum classes used by the DeformableSimplexMesh3DGradientConstraintForceFilter class.
 * \ingroup ITKDeformableMesh
 */
class DeformableSimplexMesh3DGradientConstraintForceFilterEnums
{
public:
  /**
   * \class SIDE
   * \ingroup ITKDeformableMesh
   */
  enum class SIDE : uint8_t
  {
    // half segment in direction
    NORMAL,
    // half segment in -direction
    INVERSE,
    // complete segment
    BOTH
  };
};
// Define how to print enumeration
extern ITKDeformableMesh_EXPORT std::ostream &
operator<<(std::ostream & out, const DeformableSimplexMesh3DGradientConstraintForceFilterEnums::SIDE value);

/**
 * \class DeformableSimplexMesh3DGradientConstraintForceFilter
 * \brief
 * Additional to its superclass this class reimplements the external forces methods
 * in which the scan line algorithm is used to find highest gradient is found in
 * the direction of the normal to each vertex within a specified range.
 *
 * \author Leila Baghdadi. Mouse Imaging Centre, Hospital for Sick Children, Toronto, Ontario,Canada.
 *  I acknowledge the helpful insights of Herve Delingette of INRIA, France.
 * \ingroup ITKDeformableMesh
 */

template <typename TInputMesh, typename TOutputMesh>
class ITK_TEMPLATE_EXPORT DeformableSimplexMesh3DGradientConstraintForceFilter
  : public DeformableSimplexMesh3DFilter<TInputMesh, TOutputMesh>
{
public:
  /** Standard "Self" type alias. */
  using Self = DeformableSimplexMesh3DGradientConstraintForceFilter;

  /** Standard "Superclass" type alias. */
  using Superclass = DeformableSimplexMesh3DFilter<TInputMesh, TOutputMesh>;

  /** Smart pointer type alias support */
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(DeformableSimplexMesh3DGradientConstraintForceFilter);

  /** Some type alias. */
  using InputMeshType = TInputMesh;
  using OutputMeshType = TOutputMesh;

  using typename Superclass::PointType;
  using typename Superclass::GradientIndexType;
  using typename Superclass::GradientIndexValueType;
  using typename Superclass::GradientType;
  using typename Superclass::GradientImageType;

  /* Mesh pointer definition. */
  using InputMeshPointer = typename InputMeshType::Pointer;
  using OutputMeshPointer = typename OutputMeshType::Pointer;

  using PixelType = typename InputMeshType::PixelType;

  using GradientIntensityImageType = Image<PixelType, 3>;
  using GradientIntensityImagePointer = typename GradientIntensityImageType::Pointer;

  using OriginalImageType = Image<float, 3>;
  using OriginalImageIndexType = typename OriginalImageType::IndexType;
  using ImageIndexValueType = typename OriginalImageIndexType::IndexValueType;
  using OriginalImagePointer = typename OriginalImageType::ConstPointer;

  /** control the range of search for Bresenham at normal line */
  /** @ITKStartGrouping */
  itkSetMacro(Range, int);
  itkGetConstMacro(Range, int);
  /** @ITKEndGrouping */
  using SIDEEnum = DeformableSimplexMesh3DGradientConstraintForceFilterEnums::SIDE;
#if !defined(ITK_LEGACY_REMOVE)
  /**Exposes enums values for backwards compatibility*/
  static constexpr SIDEEnum NORMAL = SIDEEnum::NORMAL;
  static constexpr SIDEEnum INVERSE = SIDEEnum::INVERSE;
  static constexpr SIDEEnum BOTH = SIDEEnum::BOTH;
#endif

  /**
   * Set Original image
   */
  itkSetConstObjectMacro(Image, OriginalImageType);

protected:
  DeformableSimplexMesh3DGradientConstraintForceFilter();
  ~DeformableSimplexMesh3DGradientConstraintForceFilter() override;
  DeformableSimplexMesh3DGradientConstraintForceFilter(const Self &) {}
  void
  operator=(const Self &)
  {}
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /**
   * Compute the external force component
   */
  void
  ComputeExternalForce(SimplexMeshGeometry * data, const GradientImageType * gradientImage) override;

  /**
   * Range of search for Bresenham algorithm (normal line at each vertex)
   */
  int m_Range{};

private:
  double
  NextVoxel(const double * pp, int * ic, double * x, double * y, double * z);

  int
  Signi(double a);

  void
  Clear();

  // line starting voxel
  ImageVoxel * m_StartVoxel{};
  // line voxels in direction
  std::vector<ImageVoxel *> m_Positive{};
  // line voxels in -direction
  std::vector<ImageVoxel *> m_Negative{};

  OriginalImagePointer m_Image{};
}; // end of class
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkDeformableSimplexMesh3DGradientConstraintForceFilter.hxx"
#endif

#endif /* __DeformableSimplexMesh3DGradientConstraintForceFilter_h */
