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
#ifndef __itkExponentialTransform_hxx
#define __itkExponentialTransform_hxx

#include "itkExponentialTransform.h"

#include "itkExponentialDisplacementFieldImageFilter.h"

namespace itk
{

template<class TScalar, unsigned int NDimensions>
ExponentialTransform<TScalar, NDimensions>
::ExponentialTransform()
{
}

template<class TScalar, unsigned int NDimensions>
ExponentialTransform<TScalar, NDimensions>::
~ExponentialTransform()
{
}

template<class TScalar, unsigned int NDimensions>
void
ExponentialTransform<TScalar, NDimensions>
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

  typedef ExponentialDisplacementFieldImageFilter<ConstantVelocityFieldType, DisplacementFieldType> ExponentiatorType;
  typename ExponentiatorType::Pointer exponentiator = ExponentiatorType::New();
  exponentiator->SetInput( updateField );
  exponentiator->SetAutomaticNumberOfIterations( true );
  exponentiator->SetComputeInverse( false );
  exponentiator->Update();

  DerivativeValueType *updatePointer = reinterpret_cast<DerivativeValueType *>( exponentiator->GetOutput()->GetBufferPointer() );

  // Add the update field to the current total field
  bool letArrayManageMemory = false;
  // Pass data pointer to required container. No copying is done.
  DerivativeType expUpdate( updatePointer, update.GetSize(), letArrayManageMemory );
  SuperSuperclass::UpdateTransformParameters( expUpdate, factor );

  //
  // Add the update field to the current total field before (optionally)
  // smoothing the total field
  //
  SuperSuperclass::UpdateTransformParameters( update, factor );

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
}

} // namespace itk

#endif
