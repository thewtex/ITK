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
#ifndef __itkDemonsImageToImageObjectMetric_hxx
#define __itkDemonsImageToImageObjectMetric_hxx

#include "itkDemonsImageToImageObjectMetric.h"

namespace itk
{

template < class TFixedImage, class TMovingImage, class TVirtualImage >
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::DemonsImageToImageObjectMetric()
{
  // We have our own GetValueAndDerivativeThreader's that we want
  // ImageToImageObjectMetric to use.
  this->m_DenseGetValueAndDerivativeThreader  = DemonsDenseGetValueAndDerivativeThreader::New();
  this->m_SparseGetValueAndDerivativeThreader = DemonsSparseGetValueAndDerivativeThreader::New();
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::~DemonsImageToImageObjectMetric()
{
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
typename DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>::MeasureType
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::GetValue() const
{
  itkExceptionMacro("GetValue not yet implemented.");
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
template <class TDomainPartitioner >
bool
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::DemonsGetValueAndDerivativeThreader< TDomainPartitioner >
::ProcessPoint(
                    const VirtualPointType &,
                    const FixedImagePointType &,
                    const FixedImagePixelType &        fixedImageValue,
                    const FixedImageGradientType &,
                    const MovingImagePointType &       mappedMovingPoint,
                    const MovingImagePixelType &       movingImageValue,
                    const MovingImageGradientType &    movingImageGradient,
                    MeasureType &                      metricValueReturn,
                    DerivativeType &                   localDerivativeReturn,
                    const ThreadIdType                 threadID) const
{
  /** Only the voxelwise contribution given the point pairs. */
  FixedImagePixelType diff = fixedImageValue - movingImageValue;
  metricValueReturn =
    vcl_fabs( diff  ) / static_cast<MeasureType>( FixedImageDimension );

  /* Use a pre-allocated jacobian object for efficiency */
  typedef JacobianType & JacobianReferenceType;
  JacobianReferenceType jacobian = this->m_MovingTransformJacobianPerThread[threadID];

  /** For dense transforms, this returns identity */
  this->m_EnclosingClass->GetMovingTransform()->ComputeJacobianWithRespectToParameters( mappedMovingPoint, jacobian );

  typedef typename DerivativeType::ValueType    DerivativeValueType;
  DerivativeValueType floatingpointcorrectionresolution = 10000.0;

  for ( unsigned int par = 0; par < this->m_EnclosingClass->GetNumberOfLocalParameters(); par++ )
    {
    double sum = 0.0;
    for ( SizeValueType dim = 0; dim < MovingImageDimension; dim++ )
      {
      sum += 2.0 * diff * jacobian(dim, par) * movingImageGradient[dim];
      }

    localDerivativeReturn[par] = sum;

    intmax_t test = static_cast<intmax_t>
            ( localDerivativeReturn[par] * floatingpointcorrectionresolution );

    localDerivativeReturn[par] = static_cast<DerivativeValueType>
                                  ( test / floatingpointcorrectionresolution );
    }
  return true;
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::DemonsDenseGetValueAndDerivativeThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & imageSubRegion,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typename VirtualImageType::ConstPointer virtualImage = enclosingClass->GetVirtualDomainImage();
  typedef ImageRegionConstIteratorWithIndex< VirtualImageType > IteratorType;
  IteratorType it( virtualImage, imageSubRegion );
  for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
    virtualIndex = it.GetIndex();
    virtualImage->TransformIndexToPhysicalPoint( virtualIndex, virtualPoint );
    this->ProcessVirtualPoint( virtualIndex, virtualPoint, threadId );
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::DemonsSparseGetValueAndDerivativeThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & indexSubRange,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typename VirtualImageType::ConstPointer virtualImage                     = enclosingClass->GetVirtualDomainImage();
  typename VirtualSampledPointSetType::ConstPointer virtualSampledPointSet = enclosingClass->GetVirtualSampledPointSet();
  typedef typename VirtualSampledPointSetType::MeshTraits::PointIdentifier ElementIdentifierType;
  const ElementIdentifierType begin = indexSubRange[0];
  const ElementIdentifierType end   = indexSubRange[1];
  for( ElementIdentifierType i = begin; i <= end; ++i )
    {
    virtualPoint = virtualSampledPointSet->GetPoint( i );
    virtualImage->TransformPhysicalPointToIndex( virtualPoint, virtualIndex );
    this->ProcessVirtualPoint( virtualIndex, virtualPoint, threadId );
    }
}

template < class TFixedImage, class TMovingImage, class TVirtualImage  >
void
DemonsImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk


#endif
