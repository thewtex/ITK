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
#ifndef __itkGaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform_hxx
#define __itkGaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform_hxx

#include "itkGaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform.h"

#include "itkImageDuplicator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImportImageFilter.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"
#include "itkGaussianOperator.h"

namespace itk
{

/**
 * Constructor
 */
template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>
::GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform()
{
  this->m_GaussianSpatialSmoothingVarianceForTheUpdateField = 3.0;
  this->m_GaussianSpatialSmoothingVarianceForTheTotalField = 0.5;
  this->m_GaussianTemporalSmoothingVarianceForTheUpdateField = 0.25;
  this->m_GaussianTemporalSmoothingVarianceForTheTotalField = 0.0;
}

/**
 * Destructor
 */
template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>::
~GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform()
{
}

template<class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>
::UpdateTransformParameters( DerivativeType & update, ScalarType factor )
{
  TimeVaryingVelocityFieldPointer velocityField = this->GetTimeVaryingVelocityField();

  const typename TimeVaryingVelocityFieldType::RegionType & bufferedRegion = velocityField->GetBufferedRegion();
  const SizeValueType numberOfPixels = bufferedRegion.GetNumberOfPixels();

  typedef ImportImageFilter<DisplacementVectorType, NDimensions + 1> ImporterType;
  const bool importFilterWillReleaseMemory = false;

  //
  // Smooth the update field
  //
  bool smoothUpdateField = true;
  if( this->m_GaussianSpatialSmoothingVarianceForTheUpdateField <= 0.0 && this->m_GaussianTemporalSmoothingVarianceForTheUpdateField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the update field." );
    smoothUpdateField = false;
    }
  if( smoothUpdateField )
    {
    itkDebugMacro( "Smooothing the update field." );

    DisplacementVectorType *updateFieldPointer = reinterpret_cast<DisplacementVectorType *>( update.data_block() );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( updateFieldPointer, numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( velocityField->GetBufferedRegion() );
    importer->SetOrigin( velocityField->GetOrigin() );
    importer->SetSpacing( velocityField->GetSpacing() );
    importer->SetDirection( velocityField->GetDirection() );

    TimeVaryingVelocityFieldPointer updateField = importer->GetOutput();
    updateField->Update();
    updateField->DisconnectPipeline();

    TimeVaryingVelocityFieldPointer updateSmoothField = this->GaussianSmoothTimeVaryingVelocityField( updateField,
      this->m_GaussianSpatialSmoothingVarianceForTheUpdateField, this->m_GaussianTemporalSmoothingVarianceForTheUpdateField );

    DerivativeValueType *updatePointer = reinterpret_cast<DerivativeValueType *>( updateSmoothField->GetBufferPointer() );

    memcpy( update.data_block(), updatePointer, sizeof( DisplacementVectorType ) * numberOfPixels );
    }

  //
  // Add the update field to the current total field before (optionally)
  // smoothing the total field
  //
  Superclass::UpdateTransformParameters( update, factor );

  //
  // Smooth the total field
  //
  bool smoothTotalField = true;
  if( this->m_GaussianSpatialSmoothingVarianceForTheTotalField <= 0.0 && this->m_GaussianTemporalSmoothingVarianceForTheTotalField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the total field." );
    smoothTotalField = false;
    }
  if( smoothTotalField )
    {
    itkDebugMacro( "Smooothing the total field." );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( velocityField->GetBufferPointer(), numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( velocityField->GetBufferedRegion() );
    importer->SetOrigin( velocityField->GetOrigin() );
    importer->SetSpacing( velocityField->GetSpacing() );
    importer->SetDirection( velocityField->GetDirection() );

    TimeVaryingVelocityFieldPointer totalField = importer->GetOutput();
    totalField->Update();
    totalField->DisconnectPipeline();

    TimeVaryingVelocityFieldPointer totalSmoothField = this->GaussianSmoothTimeVaryingVelocityField( totalField,
      this->m_GaussianSpatialSmoothingVarianceForTheTotalField, this->m_GaussianTemporalSmoothingVarianceForTheTotalField );

    memcpy( velocityField->GetBufferPointer(), totalSmoothField->GetBufferPointer(), sizeof( DisplacementVectorType ) * numberOfPixels );
    }

  if( smoothTotalField || smoothUpdateField )
    {
    this->IntegrateVelocityField();
    }
}

template<class TScalar, unsigned int NDimensions>
typename GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>::TimeVaryingVelocityFieldPointer
GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>
::GaussianSmoothTimeVaryingVelocityField( TimeVaryingVelocityFieldType *field, ScalarType spatialVariance, ScalarType temporalVariance )
{
  if( spatialVariance <= 0 && temporalVariance <= 0 )
    {
    return field;
    }

  typedef ImageDuplicator<TimeVaryingVelocityFieldType> DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage( field );
  duplicator->Update();

  TimeVaryingVelocityFieldPointer smoothField = duplicator->GetOutput();

  typedef VectorNeighborhoodOperatorImageFilter<TimeVaryingVelocityFieldType, TimeVaryingVelocityFieldType> SmootherType;
  typename SmootherType::Pointer smoother = SmootherType::New();

  for( unsigned int d = 0; d < TimeVaryingVelocityFieldDimension; d++ )
    {
    typedef GaussianOperator<DisplacementVectorValueType, NDimensions + 1> GaussianType;
    GaussianType gaussian;
    if( d < NDimensions )
      {
      gaussian.SetVariance( spatialVariance );
      }
    else
      {
      gaussian.SetVariance( temporalVariance );
      }

    if( gaussian.GetVariance() > 0.0 )
      {
      gaussian.SetMaximumError( 0.001 );
      gaussian.SetDirection( d );
      gaussian.SetMaximumKernelWidth( smoothField->GetRequestedRegion().GetSize()[d] );
      gaussian.CreateDirectional();

      smoother->SetOperator( gaussian );
      smoother->SetInput( smoothField );

      smoothField = smoother->GetOutput();
      smoothField->Update();
      smoothField->DisconnectPipeline();
      }
    }

  //make sure boundary does not move

  DisplacementVectorType zeroVector( 0.0 );

  ScalarType weight1 = 1.0;
  if( spatialVariance < 0.5 )
    {
    weight1 = 1.0 - 1.0 * ( spatialVariance / 0.5 );
    }
  ScalarType weight2 = 1.0 - weight1;

  typedef typename TimeVaryingVelocityFieldType::SizeType TimeVaryingVelocityFieldSizeType;
  typedef typename TimeVaryingVelocityFieldType::IndexType TimeVaryingVelocityFieldIndexType;
  typedef typename TimeVaryingVelocityFieldIndexType::IndexValueType IndexValueType;

  TimeVaryingVelocityFieldSizeType size = field->GetLargestPossibleRegion().GetSize();
  TimeVaryingVelocityFieldIndexType startIndex = field->GetLargestPossibleRegion().GetIndex();

  ImageRegionIteratorWithIndex<TimeVaryingVelocityFieldType> It( field, field->GetLargestPossibleRegion() );
  ImageRegionConstIteratorWithIndex<TimeVaryingVelocityFieldType> ItS( smoothField, smoothField->GetLargestPossibleRegion() );
  for( It.GoToBegin(), ItS.GoToBegin(); !It.IsAtEnd(); ++It, ++ItS )
    {
    TimeVaryingVelocityFieldIndexType index = It.GetIndex();

    bool isOnBoundary = false;
    for( SizeValueType d = 0; d < NDimensions; d++ )
      {
      if( index[d] == startIndex[d] || index[d] == static_cast<IndexValueType>( size[d] ) - startIndex[d] )
        {
        isOnBoundary = true;
        break;
        }
      }
    if( isOnBoundary )
      {
      It.Set( zeroVector );
      }
    else
      {
      It.Set( ItS.Get() * weight1 + It.Get() * weight2 );
      }
    }

  return field;
}

template <class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateTimeVaryingVelocityFieldTransform<TScalar, NDimensions>::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  os << indent << "Gaussian smoothing parameters: " << std::endl
     << indent << "Gaussian spatial smoothing variance for the update field: " << this->m_GaussianSpatialSmoothingVarianceForTheUpdateField << std::endl
     << indent << "Gaussian temporal smoothing variance for the update field: " << this->m_GaussianTemporalSmoothingVarianceForTheUpdateField << std::endl
     << indent << "Gaussian spatial smoothing variance for the total field: " << this->m_GaussianSpatialSmoothingVarianceForTheTotalField << std::endl
     << indent << "Gaussian temporal smoothing variance for the total field: " << this->m_GaussianTemporalSmoothingVarianceForTheTotalField << std::endl
     << std::endl;
}
} // namespace itk

#endif
