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
#ifndef itkBSplineDeformableTransform_h
#define itkBSplineDeformableTransform_h

#include "itkBSplineBaseTransform.h"

namespace itk
{
/** \class BSplineDeformableTransform
 *
 * \brief Deformable transform using a BSpline representation
 *
 * \note BSplineTransform is a newer version of this class, and it is
 * preferred.
 *
 * This class encapsulates a deformable transform of points from one
 * N-dimensional space to another N-dimensional space.
 * The deformation field is modeled using B-splines.
 * A deformation is defined on a sparse regular grid of control points
 * \f$ \vec{\lambda}_j \f$ and is varied by defining a deformation
 * \f$ \vec{g}(\vec{\lambda}_j) \f$ of each control point.
 * The deformation \f$ D(\vec{x}) \f$ at any point \f$ \vec{x} \f$
 * is obtained by using a B-spline interpolation kernel.
 *
 * The deformation field grid is defined by a user specified GridRegion,
 * GridSpacing and GridOrigin. Each grid/control point has associated with it
 * N deformation coefficients \f$ \vec{\delta}_j \f$, representing the N
 * directional components of the deformation. Deformation outside the grid
 * plus support region for the BSpline interpolation is assumed to be zero.
 *
 * Additionally, the user can specified an addition bulk transform \f$ B \f$
 * such that the transformed point is given by:
 * \f[ \vec{y} = B(\vec{x}) + D(\vec{x}) \f]
 *
 * The parameters for this transform is an N x N-D grid of spline coefficients.
 * The user specifies the parameters as one flat array: each N-D grid
 * is represented by an array in the same way an N-D image is represented
 * in the buffer; the N arrays are then concatenated together on form
 * a single array.
 *
 * For efficiency, this transform does not make a copy of the parameters.
 * It only keeps a pointer to the input parameters and assumes that the memory
 * is managed by the caller.
 *
 * The following illustrates the typical usage of this class:
\code
using TransformType = BSplineDeformableTransform<double,2,3>;
TransformType::Pointer transform = TransformType::New();

transform->SetGridRegion( region );
transform->SetGridSpacing( spacing );
transform->SetGridOrigin( origin );

// NB: the region must be set first before setting the parameters

TransformType::ParametersType parameters( transform->GetNumberOfParameters() );

// Fill the parameters with values

transform->SetParameters( parameters )

outputPoint = transform->TransformPoint( inputPoint );

\endcode
 *
 * An alternative way to set the B-spline coefficients is via array of
 * images. The grid region, spacing and origin information is taken
 * directly from the first image. It is assumed that the subsequent images
 * are the same buffered region. The following illustrates the API:
 *
\code

TransformType::ImageConstPointer images[2];

// Fill the images up with values

transform->SetCoefficientImages( images );
outputPoint = transform->TransformPoint( inputPoint );

\endcode
 *
 * Warning: use either the SetParameters() or SetCoefficientImages()
 * API. Mixing the two modes may results in unexpected results.
 *
 * The class is templated coordinate representation type (float or double),
 * the space dimension and the spline order.
 *
 * \ingroup ITKTransform
 *
 * \sa BSplineTransform
 *
 * \sphinx
 * \sphinxexample{Core/Transform/GlobalRegistrationTwoImagesBSpline,Global Registration Of Two Images (BSpline)}
 * \endsphinx
 */
template <typename TParametersValueType = double, unsigned int VDimension = 3, unsigned int VSplineOrder = 3>
class ITK_TEMPLATE_EXPORT BSplineDeformableTransform
  : public BSplineBaseTransform<TParametersValueType, VDimension, VSplineOrder>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(BSplineDeformableTransform);

  /** Standard class type aliases. */
  using Self = BSplineDeformableTransform;
  using Superclass = BSplineBaseTransform<TParametersValueType, VDimension, VSplineOrder>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** New macro for creation of through the object factory. */
  // Explicit New() method, used here because we need to split the itkNewMacro()
  // in order to overload the CreateAnother() method so that we can copy the m_BulkTransform
  // explicitly.
  // TODO: shouldn't it be done with the Clone() method?
  itkSimpleNewMacro(Self);
  [[nodiscard]] itk::LightObject::Pointer
  CreateAnother() const override
  {
    itk::LightObject::Pointer smartPtr;
    const Pointer             copyPtr = Self::New().GetPointer();
    // THE FOLLOWING LINE IS DIFFERENT FROM THE DEFAULT MACRO!
    copyPtr->m_BulkTransform = this->GetBulkTransform();
    smartPtr = static_cast<Pointer>(copyPtr);
    return smartPtr;
  }

  /** implement type-specific clone method*/
  itkCloneMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(BSplineDeformableTransform);

  /** Dimension of the domain space. */
  static constexpr unsigned int SpaceDimension = VDimension;

  /** The BSpline order. */
  static constexpr unsigned int SplineOrder = VSplineOrder;

  /** Standard scalar type for this class. */
  using ScalarType = TParametersValueType;

  /** Standard parameters container. */
  using typename Superclass::ParametersType;
  using typename Superclass::ParametersValueType;
  using typename Superclass::FixedParametersType;
  using typename Superclass::FixedParametersValueType;

  /** Standard Jacobian container. */
  using typename Superclass::JacobianType;
  using typename Superclass::JacobianPositionType;
  using typename Superclass::InverseJacobianPositionType;

  /** The number of parameters defining this transform. */
  using typename Superclass::NumberOfParametersType;

  /** Standard vector type for this class. */
  using typename Superclass::InputVectorType;
  using typename Superclass::OutputVectorType;

  /** Standard covariant vector type for this class. */
  using typename Superclass::InputCovariantVectorType;
  using typename Superclass::OutputCovariantVectorType;

  /** Standard vnl_vector type for this class. */
  using typename Superclass::InputVnlVectorType;
  using typename Superclass::OutputVnlVectorType;

  /** Standard coordinate point type for this class. */
  using InputPointType = Point<TParametersValueType, Self::SpaceDimension>;
  using OutputPointType = Point<TParametersValueType, Self::SpaceDimension>;


  /** This method sets the fixed parameters of the transform.
   * For a BSpline deformation transform, the parameters are the following:
   *    Grid Size, Grid Origin, and Grid Spacing
   *
   * The fixed parameters are the three times the size of the templated
   * dimensions.
   * This function has the effect of make the following calls:
   *       transform->SetGridSpacing( spacing );
   *       transform->SetGridOrigin( origin );
   *       transform->SetGridDirection( direction );
   *       transform->SetGridRegion( bsplineRegion );
   *
   * This function was added to allow the transform to work with the
   * itkTransformReader/Writer I/O filters.
   *
   */
  void
  SetFixedParameters(const FixedParametersType & passedParameters) override;

  /** Parameters as SpaceDimension number of images. */
  using typename Superclass::ImageType;
  using typename Superclass::ImagePointer;
  using typename Superclass::CoefficientImageArray;

  /** Set the array of coefficient images.
   *
   * This is an alternative API for setting the BSpline coefficients
   * as an array of SpaceDimension images. The fixed parameters are
   * taken from the first image. It is assumed that
   * the buffered region of all the subsequent images are the same
   * as the first image. Note that no error checking is done.
   *
   * Warning: use either the SetParameters() or SetCoefficientImages()
   * API. Mixing the two modes may results in unexpected results.
   */
  void
  SetCoefficientImages(const CoefficientImageArray & images) override;

  /** Typedefs for specifying the extent of the grid. */
  using typename Superclass::RegionType;

  using typename Superclass::IndexType;
  using typename Superclass::SizeType;
  using typename Superclass::SpacingType;
  using typename Superclass::DirectionType;
  using typename Superclass::OriginType;

  /** Interpolation weights function type. */
  using typename Superclass::WeightsFunctionType;

  using typename Superclass::WeightsType;
  using typename Superclass::ContinuousIndexType;

  /** Parameter index array type. */
  using typename Superclass::ParameterIndexArrayType;

  /**
   * Transform points by a BSpline deformable transformation.
   * On return, weights contains the interpolation weights used to compute the
   * deformation and indices of the x (zeroth) dimension coefficient parameters
   * in the support region used to compute the deformation.
   * Parameter indices for the i-th dimension can be obtained by adding
   * ( i * this->GetNumberOfParametersPerDimension() ) to the indices array.
   */
  using Superclass::TransformPoint;
  void
  TransformPoint(const InputPointType &    inputPoint,
                 OutputPointType &         outputPoint,
                 WeightsType &             weights,
                 ParameterIndexArrayType & indices,
                 bool &                    inside) const override;

  void
  ComputeJacobianWithRespectToParameters(const InputPointType &, JacobianType &) const override;

  /** Return the number of parameters that completely define the Transform */
  NumberOfParametersType
  GetNumberOfParameters() const override;

  /** Return the number of parameters per dimension */
  NumberOfParametersType
  GetNumberOfParametersPerDimension() const override;

  using PhysicalDimensionsType = typename Superclass::SpacingType;
  using typename Superclass::PixelType;

  using typename Superclass::MeshSizeType;

  /** Function to specify the transform domain origin. */
  virtual void
  SetGridOrigin(const OriginType &);

  /** Function to retrieve the transform domain origin. */
  itkGetConstMacro(GridOrigin, OriginType);

  /** This method specifies the grid spacing or resolution. */
  virtual void
  SetGridSpacing(const SpacingType &);

  /** This method retrieve the grid spacing or resolution. */
  itkGetConstMacro(GridSpacing, SpacingType);

  /** Function to specify the transform domain direction. */
  virtual void
  SetGridDirection(const DirectionType &);

  /** Function to retrieve the transform domain direction. */
  itkGetConstMacro(GridDirection, DirectionType);

  /** Function to specify the transform domain mesh size. */
  virtual void
  SetGridRegion(const RegionType &);

  /** Function to retrieve the transform domain mesh size. */
  itkGetConstMacro(GridRegion, RegionType);

  using BulkTransformType = Transform<TParametersValueType, Self::SpaceDimension, Self::SpaceDimension>;
  using BulkTransformPointer = typename BulkTransformType::ConstPointer;
  /** This method specifies the bulk transform to be applied.
   * The default is the identity transform.
   */
  /** @ITKStartGrouping */
  itkSetConstObjectMacro(BulkTransform, BulkTransformType);
  itkGetConstObjectMacro(BulkTransform, BulkTransformType);
  /** @ITKEndGrouping */
  /** Return the region of the grid wholly within the support region */
  itkGetConstReferenceMacro(ValidRegion, RegionType);

protected:
  /** Print contents of an BSplineDeformableTransform. */
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  BSplineDeformableTransform();
  ~BSplineDeformableTransform() override = default;

private:
  /** Construct control point grid size from transform domain information */
  void
  SetFixedParametersGridSizeFromTransformDomainInformation() const override;

  /** Construct control point grid origin from transform domain information */
  void
  SetFixedParametersGridOriginFromTransformDomainInformation() const override;

  /** Construct control point grid spacing from transform domain information */
  void
  SetFixedParametersGridSpacingFromTransformDomainInformation() const override;

  /** Construct control point grid direction from transform domain information */
  void
  SetFixedParametersGridDirectionFromTransformDomainInformation() const override;

  /** Construct control point grid size from transform domain information */
  void
  SetCoefficientImageInformationFromFixedParameters() override;

  /** Check if a continuous index is inside the valid region. */
  bool
  InsideValidRegion(ContinuousIndexType &) const override;

  /** The variables defining the coefficient grid domain for the
   * InternalParametersBuffer are taken from the m_CoefficientImages[0]
   * image, and must be kept in sync with them. by using
   * references to that instance, this is more naturally enforced
   * and does not introduce a speed penalty of dereferencing
   * through the pointers (although it does enforce some
   * internal class synchronization).
   */
  const RegionType &    m_GridRegion{};
  const OriginType &    m_GridOrigin{};
  const SpacingType &   m_GridSpacing{};
  const DirectionType & m_GridDirection;

  /** The bulk transform. */
  BulkTransformPointer m_BulkTransform{};

  RegionType m_ValidRegion{};

  /** Variables defining the interpolation support region. */
  unsigned long m_Offset{};
  bool          m_SplineOrderOdd{};
  IndexType     m_ValidRegionLast{};
  IndexType     m_ValidRegionFirst{};

  void
  UpdateValidGridRegion();

}; // class BSplineDeformableTransform
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkBSplineDeformableTransform.hxx"
#endif

#endif /* itkBSplineDeformableTransform_h */
