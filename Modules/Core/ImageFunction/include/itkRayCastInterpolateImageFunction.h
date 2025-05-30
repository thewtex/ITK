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
#ifndef itkRayCastInterpolateImageFunction_h
#define itkRayCastInterpolateImageFunction_h

#include "itkInterpolateImageFunction.h"
#include "itkTransform.h"
#include "itkNumericTraits.h"

namespace itk
{
/**
 * \class RayCastInterpolateImageFunction
 * \brief Projective interpolation of an image at specified positions.
 *
 * RayCastInterpolateImageFunction casts rays through a 3-dimensional
 * image and uses bilinear interpolation to integrate each plane of
 * voxels traversed.
 *
 * \warning This interpolator works for 3-dimensional images only.
 *
 * \ingroup ImageFunctions
 * \ingroup ITKImageFunction
 */
template <typename TInputImage, typename TCoordinate = double>
class ITK_TEMPLATE_EXPORT RayCastInterpolateImageFunction : public InterpolateImageFunction<TInputImage, TCoordinate>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(RayCastInterpolateImageFunction);

  /** Standard class type aliases. */
  using Self = RayCastInterpolateImageFunction;
  using Superclass = InterpolateImageFunction<TInputImage, TCoordinate>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Constants for the image dimensions */
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;

  /**
   * Type of the Transform Base class
   * The fixed image should be a 3D image
   */
  using TransformType = Transform<TCoordinate, 3, 3>;

  using TransformPointer = typename TransformType::Pointer;
  using InputPointType = typename TransformType::InputPointType;
  using OutputPointType = typename TransformType::OutputPointType;
  using TransformParametersType = typename TransformType::ParametersType;
  using TransformJacobianType = typename TransformType::JacobianType;

  using PixelType = typename Superclass::InputPixelType;

  using SizeType = typename TInputImage::SizeType;

  using DirectionType = Vector<TCoordinate, 3>;

  /**  Type of the Interpolator Base class */
  using InterpolatorType = InterpolateImageFunction<TInputImage, TCoordinate>;

  using InterpolatorPointer = typename InterpolatorType::Pointer;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(RayCastInterpolateImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** OutputType type alias support */
  using typename Superclass::OutputType;

  /** InputImageType type alias support */
  using typename Superclass::InputImageType;

  /** RealType type alias support */
  using typename Superclass::RealType;

  /** Dimension underlying input image. */
  static constexpr unsigned int ImageDimension = Superclass::ImageDimension;

  /** Point type alias support */
  using typename Superclass::PointType;

  /** Index type alias support */
  using typename Superclass::IndexType;

  /** ContinuousIndex type alias support */
  using typename Superclass::ContinuousIndexType;

  /** \brief
   * Interpolate the image at a point position.
   *
   * Returns the interpolated image intensity at a
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method.
   */
  OutputType
  Evaluate(const PointType & point) const override;

  /** Interpolate the image at a continuous index position
   *
   * Returns the interpolated image intensity at a
   * specified index position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * Subclasses must override this method.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method.
   */
  OutputType
  EvaluateAtContinuousIndex(const ContinuousIndexType & index) const override;

  /** Connect the Transform.
   * This Transformation is used to calculate the new focal point position.
   * */
  /** @ITKStartGrouping */
  itkSetObjectMacro(Transform, TransformType);
  itkGetModifiableObjectMacro(Transform, TransformType);
  /** @ITKEndGrouping */
  /** Connect the Interpolator. */
  itkSetObjectMacro(Interpolator, InterpolatorType);

  /** Get a pointer to the Interpolator.  */
  itkGetModifiableObjectMacro(Interpolator, InterpolatorType);

  /** The focal point or position of the ray source. */
  /** @ITKStartGrouping */
  itkSetMacro(FocalPoint, InputPointType);
  itkGetConstMacro(FocalPoint, InputPointType);
  /** @ITKEndGrouping */
  /** The threshold above which voxels along the ray path are integrated. */
  /** @ITKStartGrouping */
  itkSetMacro(Threshold, double);
  itkGetConstMacro(Threshold, double);
  /** @ITKEndGrouping */
  /** Check if a point is inside the image buffer.
   * \warning For efficiency, no validity checking of
   * the input image pointer is done. */
  bool
  IsInsideBuffer(const PointType &) const override
  {
    return true;
  }

  bool
  IsInsideBuffer(const ContinuousIndexType &) const override
  {
    return true;
  }

  bool
  IsInsideBuffer(const IndexType &) const override
  {
    return true;
  }

  SizeType
  GetRadius() const override
  {
    const InputImageType * input = this->GetInputImage();
    if (!input)
    {
      itkExceptionMacro("Input image required!");
    }
    return input->GetLargestPossibleRegion().GetSize();
  }

protected:
  RayCastInterpolateImageFunction();
  ~RayCastInterpolateImageFunction() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  TransformPointer    m_Transform{};
  InputPointType      m_FocalPoint{};
  double              m_Threshold{};
  InterpolatorPointer m_Interpolator{};

private:
  class RayCastHelper;
};
} // namespace itk


/** \class RayCastHelperEnums
 * \brief Contains all enum classes used by RayCastHelper class.
 * \ingroup ITKImageFunction
 * @tparam TInputImage
 * @tparam TCoordinate
 */
class RayCastHelperEnums
{
public:
  /**
   * \class TraversalDirection
   * \ingroup ITKImageFunction
   * The ray is traversed by stepping in the axial direction
   * that enables the greatest number of planes in the volume to be
   * intercepted.
   *
   * This enumeration is not exposed to the user, so no need to
   * create an ostream operator<< for it.
   */
  enum class TraversalDirection : uint8_t
  {
    UNDEFINED_DIRECTION = 0, //!< Undefined
    TRANSVERSE_IN_X,         //!< x
    TRANSVERSE_IN_Y,         //!< y
    TRANSVERSE_IN_Z,         //!< z
    LAST_DIRECTION
  };
};

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkRayCastInterpolateImageFunction.hxx"
#endif

#endif
