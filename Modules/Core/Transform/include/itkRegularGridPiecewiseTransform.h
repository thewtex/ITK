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
#ifndef __itkRegularGridPiecewiseTransform_h
#define __itkRegularGridPiecewiseTransform_h

#include "itkPiecewiseTransformBase.h"

#include <deque>

namespace itk
{

/** \class RegularGridPiecewiseTransform
 * \brief This class operates with spatial dependency over regularly spaced sub-transforms.
 *
 *
 * ... This class ...
 *
 * Transforms are stored in a container (queue), in the following order:
 *    \f$ T_0, T_1, ... , T_N-1 \f$
 *
 * Transforms are added individually via a single method, AddTransform(),
 * or automatically using ...
 *
 * The sub-transforms are the same dimensionality as this class.
 *
 * Indexing:
 * The index values used in GetNthTransform follow the
 * order in which transforms were added. Thus, the first transform added is at
 * index 0, the next at index 1, etc.
 *
 * \sa PiecewiseTransformBase
 * \sa MultiTransformBase
 * \ingroup ITKTransform
 */
template
<class TScalar = double, unsigned int NDimensions = 3>
class ITK_EXPORT RegularGridPiecewiseTransform :
  public PiecewiseTransformBase<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef RegularGridPiecewiseTransform                             Self;
  typedef PiecewiseTransformBase<TScalar, NDimensions, NDimensions> Superclass;
  typedef SmartPointer<Self>                                        Pointer;
  typedef SmartPointer<const Self>                                  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( RegularGridPiecewiseTransform, PiecewiseTransformBase );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

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
  typedef typename Superclass::WeightsArrayType           WeightsArrayType;

  /** Dimension of the domain spaces. */
  itkStaticConstMacro( InputDimension, unsigned int, NDimensions );
  itkStaticConstMacro( OutputDimension, unsigned int, NDimensions );

  /** Set the grid dimensions. */
  void SetGridDimensions( SizeValueType width, SizeValueType height );

  /** Fill the transform list by cloning the provided transform.
   *  The grid dimensions must already be set. */
  void SetSubTransform( const TransformTypePointer & transform );

protected:
  RegularGridPiecewiseTransform();
  virtual ~RegularGridPiecewiseTransform();
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Get lists of transforms to include and transform weights at a given point */
  virtual void GetTransformListAndWeightsAtPoint( TransformQueueType & transforms, WeightsArrayType & weights ) = 0;

  /** Clone the current transform */
  virtual typename LightObject::Pointer InternalClone() const;

private:
  RegularGridPiecewiseTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );     // purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegularGridPiecewiseTransform.hxx"
#endif

#endif // __itkRegularGridPiecewiseTransform_h
