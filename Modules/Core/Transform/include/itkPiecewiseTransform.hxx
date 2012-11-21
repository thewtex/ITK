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
#ifndef __itkPiecewiseTransform_hxx
#define __itkPiecewiseTransform_hxx

#include "itkPiecewiseTransform.h"
#include <cstring> // for memcpy on some platforms

namespace itk
{

/**
 * Constructor
 */
template
<class TScalar, unsigned int NDimensions>
PiecewiseTransform<TScalar, NDimensions>::PiecewiseTransform()
{
}

/**
 * Destructor
 */
template
<class TScalar, unsigned int NDimensions>
PiecewiseTransform<TScalar, NDimensions>::
~PiecewiseTransform()
{
}

/**
 * Transform point
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputPointType
PiecewiseTransform<TScalar, NDimensions>
::TransformPoint( const InputPointType& inputPoint ) const
{
  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformPoint( inputPoint );
    }
  else
    {
    OutputPointType outputPoint, currentPoint;
    outputPoint.Fill( NumericTraits<typename OutputPointType::ValueType>::ZeroValue() );

    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentPoint = transforms[n]->TransformPoint( inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        outputPoint[i] += currentPoint[i] * weights[n];
        }
      }

    return outputPoint;
    }
}

/**
 * Transform vector
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVectorType & ) const
{
  itkExceptionMacro( "TransformVector(Vector) unimplemented, use TransformVector(Vector,Point)" );
}

/**
 * Transform vector with position
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVectorType & inputVector, const InputPointType & inputPoint ) const
{
  OutputVectorType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputVectorType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformVector( inputVector, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformVector( inputVector, inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }
  return outputVector;
}

/**
 * Transform vector
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVnlVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVnlVectorType & inputVector, const InputPointType & inputPoint ) const
{
  OutputVnlVectorType outputVector, currentVector;
  outputVector.fill( NumericTraits<TScalar>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformVector( inputVector, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformVector( inputVector, inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform vector
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVnlVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVnlVectorType &) const
{
  itkExceptionMacro( "TransformVector(VnlVector) unimplemented, use TransformVector(VnlVector,Point)" );
}

/**
 * Transform vector with position
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVectorPixelType & ) const
{
  itkExceptionMacro( "TransformVector(VectorPixelType) unimplemented, use TransformVector(VectorPixelType,Point)" );
}

/**
 * Transform vector with position
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformVector( const InputVectorPixelType & inputVector, const InputPointType & inputPoint ) const
{
  OutputVectorPixelType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputVectorPixelType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformVector( inputVector, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformVector( inputVector, inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform covariant vector
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputCovariantVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformCovariantVector( const InputCovariantVectorType & ) const
{
  itkExceptionMacro( "TransformCovariantVector(CovariantVector) unimplemented, use TransformCovariantVector(CovariantVector,Point)" );
}

/**
 * Transform covariant vector with position
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputCovariantVectorType
PiecewiseTransform<TScalar, NDimensions>
::TransformCovariantVector( const InputCovariantVectorType & inputVector, const InputPointType & inputPoint ) const
{
  OutputCovariantVectorType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputCovariantVectorType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformCovariantVector( inputVector, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformCovariantVector( inputVector, inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform covariant vector
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformCovariantVector( const InputVectorPixelType & ) const
{
  itkExceptionMacro( "TransformCovariantVector(VectorPixelType) unimplemented, use TransformCovariantVector(VectorPixelType,Point)" );
}

/**
 * Transform covariant vector with position
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformCovariantVector( const InputVectorPixelType & inputVector, const InputPointType & inputPoint ) const
{
  OutputVectorPixelType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputVectorPixelType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformCovariantVector( inputVector, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformCovariantVector( inputVector, inputPoint );
      for( SizeValueType i=0; i < NDimensions; i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform diffusion tensor 3d
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputDiffusionTensor3DType
PiecewiseTransform<TScalar, NDimensions>
::TransformDiffusionTensor3D( const InputDiffusionTensor3DType & inputTensor, const InputPointType & inputPoint ) const
{
  OutputDiffusionTensor3DType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputDiffusionTensor3DType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformDiffusionTensor3D( inputTensor, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformDiffusionTensor3D( inputTensor, inputPoint );
      for( SizeValueType i=0; i < currentVector.Size(); i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform diffusion tensor 3d
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformDiffusionTensor3D( const InputVectorPixelType & inputTensor, const InputPointType & inputPoint ) const
{
  OutputVectorPixelType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputVectorPixelType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformDiffusionTensor3D( inputTensor, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformDiffusionTensor3D( inputTensor, inputPoint );
      for( SizeValueType i=0; i < currentVector.Size(); i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform diffusion tensor 3d
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputDiffusionTensor3DType
PiecewiseTransform<TScalar, NDimensions>
::TransformDiffusionTensor3D( const InputDiffusionTensor3DType & ) const
{
  itkExceptionMacro( "TransformVector(DiffusionTensor3DType) unimplemented, use TransformVector(DiffusionTensor3DType,Point)" );
}

/**
 * Transform diffusion tensor 3d
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformDiffusionTensor3D( const InputVectorPixelType & ) const
{
  itkExceptionMacro( "TransformDiffusionTensor3D(VectorPixelType) unimplemented, use TransformDiffusionTensor3D(VectorPixelType,Point)" );
}

/**
 * Transform ssr tensor
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputSymmetricSecondRankTensorType
PiecewiseTransform<TScalar, NDimensions>
::TransformSymmetricSecondRankTensor( const InputSymmetricSecondRankTensorType & inputTensor, const InputPointType & inputPoint ) const
{
  OutputSymmetricSecondRankTensorType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputSymmetricSecondRankTensorType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformSymmetricSecondRankTensor( inputTensor, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformSymmetricSecondRankTensor( inputTensor, inputPoint );
      for( SizeValueType i=0; i < currentVector.Size(); i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform ssr tensor
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformSymmetricSecondRankTensor( const InputVectorPixelType & inputTensor, const InputPointType & inputPoint ) const
{
  OutputVectorPixelType outputVector, currentVector;
  outputVector.Fill( NumericTraits<typename OutputVectorPixelType::ValueType>::ZeroValue() );

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( inputPoint, transforms, weights, offsets );

  SizeValueType numTransforms = transforms.size();

  if( numTransforms == 1 )
    {
    return transforms[0]->TransformSymmetricSecondRankTensor( inputTensor, inputPoint );
    }
  else
    {
    for( SizeValueType n=0; n < numTransforms; n++ )
      {
      currentVector = transforms[n]->TransformSymmetricSecondRankTensor( inputTensor, inputPoint );
      for( SizeValueType i=0; i < currentVector.Size(); i++ )
        {
        currentVector[i] *= weights[n];
        }
      outputVector += currentVector;
      }
    }

  return outputVector;
}

/**
 * Transform ssr tensor
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputSymmetricSecondRankTensorType
PiecewiseTransform<TScalar, NDimensions>
::TransformSymmetricSecondRankTensor( const InputSymmetricSecondRankTensorType & ) const
{
  itkExceptionMacro( "TransformSymmetricSecondRankTensor(SymmetricSecondRankTensorType) unimplemented, use TransformSymmetricSecondRankTensor(SymmetricSecondRankTensorType,Point)" );
}

/**
 * Transform ssr tensor
 */
template
<class TScalar, unsigned int NDimensions>
typename PiecewiseTransform<TScalar, NDimensions>
::OutputVectorPixelType
PiecewiseTransform<TScalar, NDimensions>
::TransformSymmetricSecondRankTensor( const InputVectorPixelType & ) const
{
  itkExceptionMacro( "TransformSymmetricSecondRankTensor(VectorPixelType) unimplemented, use TransformSymmetricSecondRankTensor(VectorPixelType,Point)" );
}

template
<class TScalar, unsigned int NDimensions>
void
PiecewiseTransform<TScalar, NDimensions>
::ComputeJacobianWithRespectToParameters( const InputPointType & point, JacobianType & jac ) const
{
  /* Returns a concatenated MxN array, holding the Jacobian of each sub
   * transform that is relevant at point. The order is the same
   * as queue order.
   * M rows = dimensionality of the transforms
   * N cols = number of local parameters at point. */

  TransformQueueType transforms;
  WeightArrayType weights;
  OffsetArrayType offsets;
  this->GetTransformListAndNormalizedWeightsAtPoint( point, transforms, weights, offsets );

  NumberOfParametersType offset = NumericTraits< NumberOfParametersType >::Zero;

  SizeValueType numTransforms = transforms.size();

  /* Single transform */

  if( numTransforms == 1 )
    {
    transforms[0]->ComputeJacobianWithRespectToParameters( point, jac );
    return;
    }

  /* Multiple transforms */

  jac.SetSize( NDimensions, this->GetNumberOfLocalParametersAtPoint( point ) );

  typename TransformType::JacobianType current_jacobian;

  for( SizeValueType n=0; n < numTransforms; n++ )
    {
    TransformType const * subtransform = transforms[n].GetPointer();
    NumberOfParametersType numberOfLocalParameters = subtransform->GetNumberOfLocalParametersAtPoint( point );
    current_jacobian.SetSize( NDimensions, numberOfLocalParameters );
    subtransform->ComputeJacobianWithRespectToParameters( point, current_jacobian );
    current_jacobian *= weights[n];
    jac.update( current_jacobian, 0, offset );
    offset += numberOfLocalParameters;
    }

  return;
}

template <class TScalarType, unsigned int NDimensions>
void
PiecewiseTransform<TScalarType, NDimensions>
::UpdateFullArrayWithLocalParametersAtPoint( DerivativeType & fullArray, const DerivativeType & localUpdate, const InputPointType & point ) const
{
  TransformQueueType  transforms;
  WeightArrayType     weights; // not used
  OffsetArrayType     fullOffsets;
  GetTransformListAndNormalizedWeightsAtPoint( point, transforms, weights, fullOffsets );

  /* Always do this loop and sub-arrays, even when there's only one active transform.
   * Because fullArray will still be larger than localUpdate, unless there happens to
   * only be one transform in the main queue itself. */
  OffsetValueType localOffset = 0;
  for( SizeValueType n=0; n < transforms.size(); n++ )
    {
    TransformType * subtransform = const_cast<TransformType*>( transforms[n].GetPointer() );
    DerivativeType subFullArray( &( (fullArray.data_block())[fullOffsets[n]] ), subtransform->GetNumberOfParameters(), false );
    NumberOfParametersType numLocal = subtransform->GetNumberOfLocalParametersAtPoint(point);
    DerivativeType subLocalUpdate( &( (localUpdate.data_block() )[localOffset]), numLocal, false );
    subtransform->UpdateFullArrayWithLocalParametersAtPoint( subFullArray, subLocalUpdate, point );
    localOffset += numLocal;
    }
}

} // namespace itk

#endif
