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
#ifndef __itkCentralDifferenceImageFunction_hxx
#define __itkCentralDifferenceImageFunction_hxx

#include "itkCentralDifferenceImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage, class TCoordRep, class TOutputType >
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::CentralDifferenceImageFunction()
{
  this->m_UseImageDirection = true;

  /* Interpolator. Default to linear. */
  typedef LinearInterpolateImageFunction< TInputImage, TCoordRep >
                                                  LinearInterpolatorType;
  this->m_Interpolator = LinearInterpolatorType::New();
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
void
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::SetInputImage(const TInputImage *inputData)
{
  if ( inputData != this->m_Image )
    {
    Superclass::SetInputImage( inputData );
    this->m_Interpolator->SetInputImage( inputData );
    this->Modified();
    }
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
void
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::SetInterpolator(InterpolatorType *interpolator )
{
  if ( interpolator != this->m_Interpolator )
    {
    this->m_Interpolator = interpolator;
    if( this->GetInputImage() != NULL )
      {
      this->m_Interpolator->SetInputImage( this->GetInputImage() );
      }
    this->Modified();
    }
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
void
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "UseImageDirection = " << this->m_UseImageDirection << std::endl;
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
typename CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >::OutputType
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::EvaluateAtIndex(const IndexType & index) const
{
  const InputImageType *inputImage = this->GetInputImage();
  const unsigned int numberComponents = this->GetInputImage()->GetNumberOfComponentsPerPixel();

  OutputType derivative;
  derivative.Fill( NumericTraits<OutputValueType>::ZeroValue() );

  IndexType neighIndex = index;

  const typename InputImageType::RegionType & region = inputImage->GetBufferedRegion();
  const typename InputImageType::SizeType & size     = region.GetSize();
  const typename InputImageType::IndexType & start   = region.GetIndex();

  typedef typename InputImageType::PixelType PixelType;
  const PixelType * neighPixels[Self::ImageDimension][2];

  const unsigned int MaxDims = Self::ImageDimension;
  for ( unsigned int nc = 0; nc < numberComponents; nc++)
    {
    ScalarDerivativeType componentDerivative;
    ScalarDerivativeType componentDerivativeOut;

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      // bounds checking
      if ( index[dim] < start[dim] + 1
           || index[dim] > ( start[dim] + static_cast< OffsetValueType >( size[dim] ) - 2 ) )
        {
        componentDerivative[dim] = NumericTraits<OutputValueType>::ZeroValue();
        continue;
        }

      // get pixels
      if( nc == 0 )
        {
        neighIndex[dim] += 1;
        neighPixels[dim][0] = &( inputImage->GetPixel(neighIndex) );
        neighIndex[dim] -= 2;
        neighPixels[dim][1] = &( inputImage->GetPixel(neighIndex) );
        neighIndex[dim] += 1;
        }

      // compute derivative
      componentDerivative[dim] = InputPixelConvertType::GetNthComponent( nc, *neighPixels[dim][0] );
      componentDerivative[dim] -= InputPixelConvertType::GetNthComponent( nc, *neighPixels[dim][1] );
      componentDerivative[dim] *= 0.5 / inputImage->GetSpacing()[dim];
      }

    if ( this->m_UseImageDirection )
      {
      inputImage->TransformLocalVectorToPhysicalVector(componentDerivative, componentDerivativeOut);
      }
    else
      {
      componentDerivativeOut = componentDerivative;
      }

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      OutputConvertType::SetNthComponent( nc * numberComponents + dim, derivative, componentDerivativeOut[dim] );
      }
    }

  return ( derivative );
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
typename CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >::OutputType
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::Evaluate(const PointType & point) const
{
  typedef typename PointType::ValueType           PointValueType;
  typedef typename OutputType::ValueType          DerivativeValueType;
  typedef typename ContinuousIndexType::ValueType ContinuousIndexValueType;

  const InputImageType *inputImage = this->GetInputImage();
  const unsigned int numberComponents = inputImage->GetNumberOfComponentsPerPixel();

  OutputType derivative;
  derivative.Fill( NumericTraits<OutputValueType>::ZeroValue() );

  PointType neighPoint1 = point;
  PointType neighPoint2 = point;

  const SpacingType & spacing = inputImage->GetSpacing();

  typedef typename InputImageType::PixelType PixelType;
  PixelType neighPixels[Self::ImageDimension][2];

  const unsigned int MaxDims = Self::ImageDimension;
  for ( unsigned int nc = 0; nc < numberComponents; nc++ )
    {
    ScalarDerivativeType componentDerivative;
    ScalarDerivativeType componentDerivativeOut;

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      PointValueType offset = static_cast<PointValueType>(0.5) * spacing[dim];

      // Check the bounds using the point because the image direction may swap dimensions,
      // making checks in index space inaccurate.
      // If on a boundary, we set the derivative to zero. This is done to match the behavior
      // of EvaluateAtIndex. Another approach is to calculate the 1-sided difference.
      neighPoint1[dim] = point[dim] - offset;
      if( ! this->IsInsideBuffer( neighPoint1 ) )
        {
        componentDerivative[dim] = NumericTraits<OutputValueType>::Zero;
        neighPoint1[dim] = point[dim];
        neighPoint2[dim] = point[dim];
        continue;
        }
      neighPoint2[dim] = point[dim] + offset;
      if( ! this->IsInsideBuffer( neighPoint2 ) )
        {
        componentDerivative[dim] = NumericTraits<OutputValueType>::Zero;
        neighPoint1[dim] = point[dim];
        neighPoint2[dim] = point[dim];
        continue;
        }

      PointValueType delta = neighPoint2[dim] - neighPoint1[dim];
      if( delta > 10.0 * NumericTraits<PointValueType>::epsilon() )
        {
        if( nc == 0 )
          {
          neighPixels[dim][0] = this->m_Interpolator->Evaluate( neighPoint2 );
          neighPixels[dim][1] = this->m_Interpolator->Evaluate( neighPoint1 );
          }
        OutputValueType left = InputPixelConvertType::GetNthComponent( nc, neighPixels[dim][0] );
        OutputValueType right = InputPixelConvertType::GetNthComponent( nc, neighPixels[dim][1] );
        componentDerivative[dim] = (left - right) / delta;
        }
      else
        {
        componentDerivative[dim] = NumericTraits<OutputValueType>::ZeroValue();
        }

      neighPoint1[dim] = point[dim];
      neighPoint2[dim] = point[dim];
      }

    // Since we've implicitly calculated the derivative with respect to image
    // direction, we need to reorient into index-space if the user
    // desires.
    if ( ! this->m_UseImageDirection )
      {
      inputImage->TransformPhysicalVectorToLocalVector(componentDerivative, componentDerivativeOut);
      }
    else
      {
      componentDerivativeOut = componentDerivative;
      }

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      OutputConvertType::SetNthComponent( nc * numberComponents + dim, derivative, componentDerivativeOut[dim] );
      }
    }

  return derivative;
}

/**
 *
 */
template< class TInputImage, class TCoordRep, class TOutputType >
typename CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >::OutputType
CentralDifferenceImageFunction< TInputImage, TCoordRep, TOutputType >
::EvaluateAtContinuousIndex(const ContinuousIndexType & cindex) const
{
  typedef typename OutputType::ValueType          DerivativeValueType;
  typedef typename ContinuousIndexType::ValueType ContinuousIndexValueType;

  const InputImageType *inputImage = this->GetInputImage();
  const unsigned int numberComponents = inputImage->GetNumberOfComponentsPerPixel();

  OutputType derivative;
  derivative.Fill( NumericTraits<ContinuousIndexValueType>::ZeroValue() );

  ContinuousIndexType neighIndex = cindex;
  const typename InputImageType::RegionType & region =
    inputImage->GetBufferedRegion();

  const typename InputImageType::SizeType & size   = region.GetSize();
  const typename InputImageType::IndexType & start = region.GetIndex();

  typedef typename InputImageType::PixelType PixelType;
  PixelType neighPixels[Self::ImageDimension][2];

  const unsigned int MaxDims = Self::ImageDimension;
  for ( unsigned int nc = 0; nc < numberComponents; nc++)
    {
    ScalarDerivativeType componentDerivative;
    ScalarDerivativeType componentDerivativeOut;

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      // bounds checking
      if ( cindex[dim] < static_cast<ContinuousIndexValueType>(start[dim] + 1)
           || cindex[dim] > static_cast<ContinuousIndexValueType>
            ( start[dim] + static_cast< OffsetValueType >( size[dim] ) - 2 ) )
        {
        componentDerivative[dim] = NumericTraits<DerivativeValueType>::ZeroValue();
        continue;
        }

      // get pixels
      if( nc == 0 )
        {
        neighIndex[dim] += static_cast<ContinuousIndexValueType>(1.0);
        neighPixels[dim][0] = this->m_Interpolator->EvaluateAtContinuousIndex(neighIndex);
        neighIndex[dim] -= static_cast<ContinuousIndexValueType>(2.0);
        neighPixels[dim][1] = this->m_Interpolator->EvaluateAtContinuousIndex(neighIndex);
        neighIndex[dim] += static_cast<ContinuousIndexValueType>(1.0);
        }

      // compute derivative
      componentDerivative[dim] = InputPixelConvertType::GetNthComponent(nc, neighPixels[dim][0] );
      componentDerivative[dim] -= InputPixelConvertType::GetNthComponent(nc, neighPixels[dim][1] );
      componentDerivative[dim] *= static_cast<ContinuousIndexValueType>(0.5) / inputImage->GetSpacing()[dim];
      }

    if ( this->m_UseImageDirection )
      {
      inputImage->TransformLocalVectorToPhysicalVector(componentDerivative, componentDerivativeOut);
      }
    else
      {
      componentDerivativeOut = componentDerivative;
      }

    for ( unsigned int dim = 0; dim < MaxDims; dim++ )
      {
      OutputConvertType::SetNthComponent( nc * numberComponents + dim, derivative, componentDerivativeOut[dim] );
      }
    }

  return ( derivative );
}

} // end namespace itk

#endif
