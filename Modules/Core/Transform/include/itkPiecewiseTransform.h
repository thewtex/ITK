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
#ifndef __itkPiecewiseTransform_h
#define __itkPiecewiseTransform_h

#include "itkMultiTransform.h"

#include <deque>

namespace itk
{

/** \class PiecewiseTransform
 * \brief Base class for transforms that operate with spatial dependency and weights.
 *
 *
 * ... This class ...
 *
 * Transform(s) 'active' at a given point.
 *
 * Because this is a local transform, methods that have a version that takes
 * a point must be used, such as TransformVector(), TransformCovariantVector(), etc.
 *
 * \sa MultiTransform
 * \ingroup ITKTransform
 */
template
<class TScalar = double, unsigned int NDimensions = 3>
class ITK_EXPORT PiecewiseTransform :
  public MultiTransform<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef PiecewiseTransform                                Self;
  typedef MultiTransform<TScalar, NDimensions, NDimensions> Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( PiecewiseTransform, MultiTransform );

  /** Sub transform type **/
  typedef typename Superclass::TransformType                TransformType;
  typedef typename Superclass::TransformTypePointer         TransformTypePointer;
  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer  InverseTransformBasePointer;
  /** Scalar type. */
  typedef typename Superclass::ScalarType                 ScalarType;
  /** Parameters type. */
  typedef typename Superclass::ParametersType             ParametersType;
  typedef typename Superclass::ParametersValueType        ParametersValueType;
  /** Derivative type */
  typedef typename Superclass::DerivativeType             DerivativeType;
  /** Jacobian type. */
  typedef typename Superclass::JacobianType               JacobianType;
  /** Transform category type. */
  typedef typename Superclass::TransformCategoryType      TransformCategoryType;
  /** Standard coordinate point type for this class. */
  typedef typename Superclass::InputPointType             InputPointType;
  typedef typename Superclass::OutputPointType            OutputPointType;
  /** Standard vector type for this class. */
  typedef typename Superclass::InputVectorType            InputVectorType;
  typedef typename Superclass::OutputVectorType           OutputVectorType;
  /** Standard covariant vector type for this class */
  typedef typename Superclass::InputCovariantVectorType   InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType  OutputCovariantVectorType;
  /** Standard vnl_vector type for this class. */
  typedef typename Superclass::InputVnlVectorType         InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType        OutputVnlVectorType;
  /** Standard Vectorpixel type for this class */
  typedef typename Superclass::InputVectorPixelType       InputVectorPixelType;
  typedef typename Superclass::OutputVectorPixelType      OutputVectorPixelType;
  /** Standard DiffusionTensor3D typedef for this class */
  typedef typename Superclass::InputDiffusionTensor3DType  InputDiffusionTensor3DType;
  typedef typename Superclass::OutputDiffusionTensor3DType OutputDiffusionTensor3DType;
  /** Standard SymmetricSecondRankTensor typedef for this class */
  typedef typename Superclass::InputSymmetricSecondRankTensorType   InputSymmetricSecondRankTensorType;
  typedef typename Superclass::OutputSymmetricSecondRankTensorType  OutputSymmetricSecondRankTensorType;

  /** Transform queue type */
  typedef typename Superclass::TransformQueueType         TransformQueueType;

  /** The number of parameters defininig this transform. */
  typedef typename Superclass::NumberOfParametersType     NumberOfParametersType;

  /** Array type for returning transform weights */
  typedef Array<TScalar>                                  WeightArrayType;

  /** Array type for returning transform parameter offsets */
  typedef Array<IndexValueType>                           OffsetArrayType;

  /** Dimension of the domain spaces. */
  itkStaticConstMacro( InputDimension, unsigned int, NDimensions );
  itkStaticConstMacro( OutputDimension, unsigned int, NDimensions );

  /** Compute the position of point in the new space.
  *
  * ...
  */
  virtual OutputPointType TransformPoint( const InputPointType & inputPoint ) const;

  /**  Method to transform a vector. */
  using Superclass::TransformVector;
  virtual OutputVectorType      TransformVector(const InputVectorType &) const;
  virtual OutputVnlVectorType   TransformVector(const InputVnlVectorType & inputVector) const;
  virtual OutputVectorPixelType TransformVector(const InputVectorPixelType & inputVector ) const;
  virtual OutputVectorType      TransformVector(const InputVectorType & inputVector, const InputPointType & inputPoint ) const;
  virtual OutputVnlVectorType   TransformVector(const InputVnlVectorType & inputVector, const InputPointType & inputPoint ) const;
  virtual OutputVectorPixelType TransformVector(const InputVectorPixelType & inputVector, const InputPointType & inputPoint ) const;

  /**  Method to transform a CovariantVector. */
  using Superclass::TransformCovariantVector;
  virtual OutputCovariantVectorType TransformCovariantVector(const InputCovariantVectorType &) const;
  virtual OutputVectorPixelType     TransformCovariantVector(const InputVectorPixelType &) const;
  virtual OutputCovariantVectorType TransformCovariantVector(const InputCovariantVectorType & inputVector, const InputPointType & inputPoint ) const;
  virtual OutputVectorPixelType     TransformCovariantVector(const InputVectorPixelType & inputVector, const InputPointType & inputPoint ) const;

  /** Method to transform a DiffusionTensor3D */
  using Superclass::TransformDiffusionTensor3D;
  virtual OutputDiffusionTensor3DType TransformDiffusionTensor3D( const InputDiffusionTensor3DType & inputTensor) const;
  virtual OutputVectorPixelType       TransformDiffusionTensor3D( const InputVectorPixelType & inputTensor) const;
  virtual OutputDiffusionTensor3DType TransformDiffusionTensor3D( const InputDiffusionTensor3DType & inputTensor, const InputPointType & inputPoint ) const;
  virtual OutputVectorPixelType       TransformDiffusionTensor3D( const InputVectorPixelType & inputTensor, const InputPointType & inputPoint ) const;

  /** Method to transform a SymmetricSecondRankTensor */
  using Superclass::TransformSymmetricSecondRankTensor;
  virtual OutputSymmetricSecondRankTensorType TransformSymmetricSecondRankTensor( const InputSymmetricSecondRankTensorType & inputTensor) const;
  virtual OutputVectorPixelType               TransformSymmetricSecondRankTensor( const InputVectorPixelType & inputTensor) const;
  virtual OutputSymmetricSecondRankTensorType TransformSymmetricSecondRankTensor( const InputSymmetricSecondRankTensorType & inputTensor, const InputPointType & inputPoint ) const;
  virtual OutputVectorPixelType               TransformSymmetricSecondRankTensor( const InputVectorPixelType & inputTensor, const InputPointType & inputPoint ) const;

  /**
   * \copydoc Transform::ComputeJacobianWithRespectToParameters()
   *
   * The returned jacobian contains a concatenation of the weighted jacobians for
   * only the sub-transforms 'active' at point \c p. Each jacobian is
   * weighted by the sub-transforms weight at that point.
   */
  virtual void ComputeJacobianWithRespectToParameters(const InputPointType  & p, JacobianType & j) const;

  /**
   * \copydoc Transform::UpdateFullArrayWithLocalParametersAtPoint()
   *
   * \note Weights are not applied. The localUpdate is added directly to fullArray.
   * \note The order of the values in \c localUpdate parameter is expected to
   * match the order of the sub-transforms as returned by GetTransformListAndNormalizedWeightsAtPoint().
   */
  virtual void UpdateFullArrayWithLocalParametersAtPoint( DerivativeType & fullArray, const DerivativeType & localUpdate, const InputPointType & point ) const;

  /** Get transform category */
  virtual TransformCategoryType GetTransformCategory() const
  {
    return Self::Piecewise;
  }

  /** Allow the transform to perform any required initialization steps.
   *  This must be called by the user after setting all options and parameters.
   *  Derived classes must override as needed.
   */
  virtual void Initialize()
  {
  }

  /* Redeclare these methods as pure virtual to ensure they're redefined in derived classes */
  virtual NumberOfParametersType GetNumberOfLocalParametersAtPoint(const InputPointType  &) const = 0;
  virtual bool GetNumberOfLocalParametersAtPointIsConstant() const = 0;

protected:
  PiecewiseTransform();
  virtual ~PiecewiseTransform();

  /** Clone the current transform. There's nothing in this base class to clone.
   *  Make this pure virtual to force dervied classes to define. */
  virtual typename LightObject::Pointer InternalClone() const = 0;

  /** For the given point, get lists of the active transforms to include, and the \b normalized transform weights.
   *  The weights are normalized for the particular point, not globally.
   *  \param point The point at which to get the active transforms and weights.
   *  \param transforms will hold a resized list of active transforms on return.
   *  \param weights will hold a resized array of weight values on return.
   *  \param fullOffsets hold a resized array of offsets for each active transform into
   *  the full-sized parameter array. That is, it holds each active transform's offset into the array
   *  returned by GetParameters().
   *
   *  Derived classes must implement this.
   */
 virtual void GetTransformListAndNormalizedWeightsAtPoint( const InputPointType & point, TransformQueueType & transforms, WeightArrayType & weights, OffsetArrayType & fullOffsets ) const = 0;

private:
  PiecewiseTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );     // purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPiecewiseTransform.hxx"
#endif

#endif // __itkPiecewiseTransform_h
