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
#ifndef __itkGaussianExponentialDiffeomorphicTransform_hxx
#define __itkGaussianExponentialDiffeomorphicTransform_hxx

#include "itkGaussianExponentialDiffeomorphicTransform.h"

#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkExponentialDisplacementFieldImageFilter.h"
#include "itkMultiplyImageFilter.h"

namespace itk
{

template<class TScalar, unsigned int NDimensions>
GaussianExponentialDiffeomorphicTransform<TScalar, NDimensions>
::GaussianExponentialDiffeomorphicTransform():
  m_CalculateNumberOfIterationsAutomatically( true ),
  m_MaximumNumberOfIterations( 10 ),
  m_ComputeInverse( false )
{
}

template<class TScalar, unsigned int NDimensions>
GaussianExponentialDiffeomorphicTransform<TScalar, NDimensions>::
~GaussianExponentialDiffeomorphicTransform()
{
}

template<class TScalar, unsigned int NDimensions>
void
GaussianExponentialDiffeomorphicTransform<TScalar, NDimensions>
::UpdateTransformParameters( const DerivativeType & update, ScalarType factor )
{
  DisplacementFieldPointer displacementField = this->GetDisplacementField();

  const typename DisplacementFieldType::RegionType & bufferedRegion = displacementField->GetBufferedRegion();
  const SizeValueType numberOfPixels = bufferedRegion.GetNumberOfPixels();

  typedef ImportImageFilter<DisplacementVectorType, NDimensions> ImporterType;
  const bool importFilterWillReleaseMemory = false;

  //
  // Smooth the update field
  //
  bool smoothUpdateField = true;
  if( this->m_GaussianSmoothingVarianceForTheUpdateField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the update field." );
    smoothUpdateField = false;
    }

  DisplacementVectorType *updateFieldPointer = reinterpret_cast<DisplacementVectorType *>( const_cast<DerivativeType &>( update ).data_block() );

  typename ImporterType::Pointer importer = ImporterType::New();
  importer->SetImportPointer( updateFieldPointer, numberOfPixels, importFilterWillReleaseMemory );
  importer->SetRegion( displacementField->GetBufferedRegion() );
  importer->SetOrigin( displacementField->GetOrigin() );
  importer->SetSpacing( displacementField->GetSpacing() );
  importer->SetDirection( displacementField->GetDirection() );

  DisplacementFieldPointer updateField = importer->GetOutput();
  updateField->Update();
  updateField->DisconnectPipeline();

  if( smoothUpdateField )
    {
    itkDebugMacro( "Smooothing the update field." );

    // The update field is the velocity field but since it's constant
    // we smooth it using the parent class smoothing functionality

    ConstantVelocityFieldPointer updateSmoothField = this->GaussianSmoothDisplacementField( updateField, this->GetGaussianSmoothingVarianceForTheUpdateField() );

    updateField = updateSmoothField;
    }

  // Rescale the field by the max norm

  typename DisplacementFieldType::SpacingType spacing = displacementField->GetSpacing();

  ImageRegionIterator<DisplacementFieldType> ItF( updateField, updateField->GetRequestedRegion() );

  ScalarType maxNorm = NumericTraits<ScalarType>::NonpositiveMin();
  for( ItF.GoToBegin(); !ItF.IsAtEnd(); ++ItF )
    {
    DisplacementVectorType vector = ItF.Get();

    ScalarType localNorm = 0;
    for( unsigned int d = 0; d < NDimensions; d++ )
      {
      localNorm += vnl_math_sqr( vector[d] / spacing[d] );
      }
    localNorm = vcl_sqrt( localNorm );

    if( localNorm > maxNorm )
      {
      maxNorm = localNorm;
      }
    }

  ScalarType scale = 1.0 / maxNorm;

  typedef Image<ScalarType, NDimensions> RealImageType;

  typedef MultiplyImageFilter<DisplacementFieldType, RealImageType, DisplacementFieldType> MultiplierType;
  typename MultiplierType::Pointer multiplier = MultiplierType::New();
  multiplier->SetInput( updateField );
  multiplier->SetConstant( scale );

  typename DisplacementFieldType::Pointer scaledUpdateField = multiplier->GetOutput();
  scaledUpdateField->Update();
  scaledUpdateField->DisconnectPipeline();

  typedef ExponentialDisplacementFieldImageFilter<ConstantVelocityFieldType, DisplacementFieldType> ExponentiatorType;
  typename ExponentiatorType::Pointer exponentiator = ExponentiatorType::New();
  exponentiator->SetInput( scaledUpdateField );
  exponentiator->SetAutomaticNumberOfIterations( this->m_CalculateNumberOfIterationsAutomatically );
  exponentiator->SetMaximumNumberOfIterations( this->m_MaximumNumberOfIterations );
  exponentiator->SetComputeInverse( false );

  typename DisplacementFieldType::Pointer exponentiatedField = exponentiator->GetOutput();
  exponentiatedField->Update();
  exponentiatedField->DisconnectPipeline();

  DerivativeValueType *updatePointer = reinterpret_cast<DerivativeValueType *>( exponentiatedField->GetBufferPointer() );

  // Add the update field to the current total field
  bool letArrayManageMemory = false;
  // Pass data pointer to required container. No copying is done.
  DerivativeType expUpdate( updatePointer, update.GetSize(), letArrayManageMemory );
  SuperSuperclass::UpdateTransformParameters( expUpdate, factor );

  //
  // Smooth the total field
  //
  bool smoothTotalField = true;
  if( this->m_GaussianSmoothingVarianceForTheTotalField <= 0.0 )
    {
    itkDebugMacro( "Not smoothing the total field." );
    smoothTotalField = false;
    }
  if( smoothTotalField )
    {
    itkDebugMacro( "Smoothing the total field." );

    typename ImporterType::Pointer importer2 = ImporterType::New();
    importer2->SetImportPointer( displacementField->GetBufferPointer(), numberOfPixels, importFilterWillReleaseMemory );
    importer2->SetRegion( displacementField->GetBufferedRegion() );
    importer2->SetOrigin( displacementField->GetOrigin() );
    importer2->SetSpacing( displacementField->GetSpacing() );
    importer2->SetDirection( displacementField->GetDirection() );

    DisplacementFieldPointer totalField = importer2->GetOutput();
    totalField->Update();
    totalField->DisconnectPipeline();

    DisplacementFieldPointer totalSmoothField = this->GaussianSmoothDisplacementField( totalField, this->GetGaussianSmoothingVarianceForTheTotalField() );

    ImageAlgorithm::Copy<DisplacementFieldType, DisplacementFieldType>( totalSmoothField, totalField, totalSmoothField->GetBufferedRegion(), totalField->GetBufferedRegion() );
    }

  // Compute the inverse displacement field if requested

  if( this->m_ComputeInverse )
    {

    // Create identity field if the inverse displacement field doesn't exist.

    if( !this->GetInverseDisplacementField() )
      {
      DisplacementVectorType zeroVector( 0.0 );

      typename DisplacementFieldType::Pointer identityField = DisplacementFieldType::New();
      identityField->CopyInformation( displacementField );
      identityField->SetRegions( displacementField->GetRequestedRegion() );
      identityField->Allocate();
      identityField->FillBuffer( zeroVector );

      this->SetInverseDisplacementField( identityField );
      }

    typename ExponentiatorType::Pointer exponentiatorInv = ExponentiatorType::New();
    exponentiatorInv->SetInput( scaledUpdateField );
    exponentiatorInv->SetAutomaticNumberOfIterations( this->m_CalculateNumberOfIterationsAutomatically );
    exponentiatorInv->SetMaximumNumberOfIterations( this->m_MaximumNumberOfIterations );
    exponentiatorInv->SetComputeInverse( true );
    exponentiatorInv->Update();

    typedef ComposeDisplacementFieldsImageFilter<DisplacementFieldType, DisplacementFieldType> ComposerType;
    typename ComposerType::Pointer composer = ComposerType::New();
    composer->SetDisplacementField( exponentiatorInv->GetOutput() );
    composer->SetWarpingField( this->GetInverseDisplacementField() );
    composer->Update();

    //
    // Smooth the total field
    //
    bool smoothTotalInvField = true;
    if( this->m_GaussianSmoothingVarianceForTheTotalField <= 0.0 )
      {
      itkDebugMacro( "Not smoothing the total inverse field." );
      smoothTotalInvField = false;

      this->SetInverseDisplacementField( composer->GetOutput() );
      }
    if( smoothTotalInvField )
      {
      itkDebugMacro( "Smoothing the total inverse field." );

      DisplacementFieldPointer totalSmoothInvField = this->GaussianSmoothDisplacementField( composer->GetOutput(), this->GetGaussianSmoothingVarianceForTheTotalField() );

      this->SetInverseDisplacementField( totalSmoothInvField );
      }
    }
}

/**
 * Standard "PrintSelf" method
 */
template<class TScalar, unsigned int NDimensions>
void
GaussianExponentialDiffeomorphicTransform<TScalar, NDimensions>
::PrintSelf( std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Calculate number of iterations automatically = " << this->m_CalculateNumberOfIterationsAutomatically << std::endl;
  os << indent << "Number of iterations = " << this->m_MaximumNumberOfIterations << std::endl;
  os << indent << "Compute inverse = " << this->m_ComputeInverse << std::endl;
}

} // namespace itk

#endif
