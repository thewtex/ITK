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
#ifndef __itkTimeVaryingVelocityFieldIntegrationImageFilter_hxx
#define __itkTimeVaryingVelocityFieldIntegrationImageFilter_hxx

#include "itkTimeVaryingVelocityFieldIntegrationImageFilter.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkVectorLinearInterpolateImageFunction.h"

namespace itk
{

/*
 * TimeVaryingVelocityFieldIntegrationImageFilter class definitions
 */
template<class TTimeVaryingVelocityField, class TDisplacementField>
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::TimeVaryingVelocityFieldIntegrationImageFilter() :
  m_LowerTimeBound( 0.0 ),
  m_UpperTimeBound( 1.0 ),
  m_NumberOfIntegrationSteps( 10 )
{
  this->SetNumberOfRequiredInputs( 1 );

  if( InputImageDimension - 1 != OutputImageDimension )
    {
    itkExceptionMacro( "The time-varying velocity field (input) should have "
      << "dimensionality of 1 greater than the deformation field (output). " );
    }

  typedef VectorLinearInterpolateImageFunction<TimeVaryingVelocityFieldType,
    RealType> DefaultVelocityFieldInterpolatorType;
  typename DefaultVelocityFieldInterpolatorType::Pointer
    velocityFieldInterpolator = DefaultVelocityFieldInterpolatorType::New();
  this->m_VelocityFieldInterpolator = velocityFieldInterpolator;

  typedef VectorLinearInterpolateImageFunction<DisplacementFieldType,
    RealType> DefaultDisplacementFieldInterpolatorType;
  typename DefaultDisplacementFieldInterpolatorType::Pointer
    deformationFieldInterpolator =
    DefaultDisplacementFieldInterpolatorType::New();
  this->m_DisplacementFieldInterpolator = deformationFieldInterpolator;
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::~TimeVaryingVelocityFieldIntegrationImageFilter()
{
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
void
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::GenerateOutputInformation()
{
  typename TimeVaryingVelocityFieldType::ConstPointer input = this->GetInput();
  typename DisplacementFieldType::Pointer output = this->GetOutput();

  if( !input || !output )
    {
    return;
    }

  typename DisplacementFieldType::SizeType size;
  typename DisplacementFieldType::SpacingType spacing;
  typename DisplacementFieldType::PointType origin;
  typename DisplacementFieldType::DirectionType direction;

  for( unsigned int i = 0; i < OutputImageDimension; i++ )
    {
    size[i] = input->GetRequestedRegion().GetSize()[i];
    spacing[i] = input->GetSpacing()[i];
    origin[i] = input->GetOrigin()[i];

    for( unsigned int j = 0; j < OutputImageDimension; j++ )
      {
      direction[i][j] = input->GetDirection()[i][j];
      }
    }

  output->SetOrigin( origin );
  output->SetSpacing( spacing );
  output->SetDirection( direction );
  output->SetRegions( size );
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
void
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::BeforeThreadedGenerateData()
{
  VectorType zeroVector( 0.0 );

  this->AllocateOutputs();
  this->GetOutput()->FillBuffer( zeroVector );

  this->m_VelocityFieldInterpolator->SetInputImage( this->GetInput() );

  if( !this->m_InitialDiffeomorphism.IsNull() )
    {
    this->m_DisplacementFieldInterpolator->SetInputImage(
      this->m_InitialDiffeomorphism );
    }
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
void
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::ThreadedGenerateData( const OutputRegionType &region,
  ThreadIdType itkNotUsed( threadId ) )
{
  if( this->m_LowerTimeBound == this->m_UpperTimeBound ||
    this->m_NumberOfIntegrationSteps == 0 )
    {
    return;
    }

  ImageRegionIteratorWithIndex<DisplacementFieldType> It( this->GetOutput(),
    region );
  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
    PointType point;
    this->GetOutput()->TransformIndexToPhysicalPoint( It.GetIndex(), point );
    VectorType displacement = this->IntegrateVelocityAtPoint( point );
    It.Set( displacement );
    }
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
typename TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>::VectorType
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::IntegrateVelocityAtPoint( const PointType initialSpatialPoint )
{
  // Solve the initial value problem using fourth-order Runge-Kutta
  //    y' = f(t, y), y(t_0) = y_0

  VectorType zeroVector;
  zeroVector.Fill( 0.0 );

  // Initial conditions

  PointType spatialPoint = initialSpatialPoint;
  if( !this->m_InitialDiffeomorphism.IsNull() )
    {
    if( this->m_DisplacementFieldInterpolator->IsInsideBuffer( spatialPoint ) )
      {
      spatialPoint += this->m_DisplacementFieldInterpolator->Evaluate( spatialPoint );
      }
    }

  // Perform the integration

  // Need to know how to map the time dimension of the input image to the
  // assumed domain of [0,1].

  RealType timeOrigin = this->GetInput()->GetOrigin()[InputImageDimension-1];
  RealType timeDomain = this->GetInput()->GetSpacing()[InputImageDimension-1] *
    static_cast<RealType>( this->GetInput()->
    GetLargestPossibleRegion().GetSize()[InputImageDimension-1] - 1 );

  // Calculate the delta time used for integration
  RealType deltaTime = ( timeDomain - timeOrigin ) * ( this->m_UpperTimeBound -
    this->m_LowerTimeBound ) / static_cast<RealType>( this->m_NumberOfIntegrationSteps );

  if( deltaTime == 0.0 )
    {
    return zeroVector;
    }

  RealType timePoint = timeOrigin + this->m_LowerTimeBound * timeDomain;
  for( unsigned int n = 0; n < this->m_NumberOfIntegrationSteps; n++ )
    {
    typename TimeVaryingVelocityFieldType::PointType x1;
    for( unsigned int d = 0; d < OutputImageDimension; d++ )
      {
      x1[d] = spatialPoint[d];
      }
    x1[OutputImageDimension] = timePoint;
    VectorType f1 = zeroVector;
    if( this->m_VelocityFieldInterpolator->IsInsideBuffer( x1 ) )
      {
      f1 = this->m_VelocityFieldInterpolator->Evaluate( x1 ) * deltaTime / timeDomain;
      }

    typename TimeVaryingVelocityFieldType::PointType x2;
    for( unsigned int d = 0; d < OutputImageDimension; d++ )
      {
      x2[d] = spatialPoint[d] + f1[d] * 0.5;
      }
    x2[OutputImageDimension] = timePoint + 0.5 * deltaTime;
    VectorType f2 = zeroVector;
    if( this->m_VelocityFieldInterpolator->IsInsideBuffer( x2 ) )
      {
      f2 = this->m_VelocityFieldInterpolator->Evaluate( x2 ) * deltaTime / timeDomain;
      }

    typename TimeVaryingVelocityFieldType::PointType x3;
    for( unsigned int d = 0; d < OutputImageDimension; d++ )
      {
      x3[d] = spatialPoint[d] + f2[d] * 0.5;
      }
    x3[OutputImageDimension] = timePoint + 0.5 * deltaTime;
    VectorType f3 = zeroVector;
    if( this->m_VelocityFieldInterpolator->IsInsideBuffer( x3 ) )
      {
      f3 = this->m_VelocityFieldInterpolator->Evaluate( x3 ) * deltaTime / timeDomain;
      }

    typename TimeVaryingVelocityFieldType::PointType x4;
    for( unsigned int d = 0; d < OutputImageDimension; d++ )
      {
      x4[d] = spatialPoint[d] + f3[d];
      }
    x4[OutputImageDimension] = timePoint + deltaTime;
    VectorType f4 = zeroVector;
    if( this->m_VelocityFieldInterpolator->IsInsideBuffer( x4 ) )
      {
      f4 = this->m_VelocityFieldInterpolator->Evaluate( x4 ) * deltaTime / timeDomain;
      }

    spatialPoint += ( ( f1 + f2 * 2.0 + f3 * 2.0 + f4 ) / 6.0 );
    timePoint += deltaTime;
    }

  VectorType displacement = spatialPoint - initialSpatialPoint;

  return displacement;
}

template<class TTimeVaryingVelocityField, class TDisplacementField>
void
TimeVaryingVelocityFieldIntegrationImageFilter
  <TTimeVaryingVelocityField, TDisplacementField>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "VelocityFieldInterpolator: "
    << this->m_VelocityFieldInterpolator << std::endl;
  os << indent << "LowerTimeBound: " << this->m_LowerTimeBound << std::endl;
  os << indent << "UpperTimeBound: " << this->m_UpperTimeBound << std::endl;
  os << indent << "NumberOfIntegrationSteps: "
    << this->m_NumberOfIntegrationSteps << std::endl;

  if( !this->m_InitialDiffeomorphism.IsNull() )
    {
    os << indent << "InitialDiffeomorphism: " << this->m_InitialDiffeomorphism << std::endl;
    os << indent << "DisplacementFieldInterpolator: "
      << this->m_DisplacementFieldInterpolator << std::endl;
    }
}

}  //end namespace itk

#endif
