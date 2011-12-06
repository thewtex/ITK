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
#ifndef __itkGaussianSmoothingOnUpdateDisplacementFieldTransform_hxx
#define __itkGaussianSmoothingOnUpdateDisplacementFieldTransform_hxx

#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransform.h"

#include "itkAddImageFilter.h"
#include "itkGaussianOperator.h"
#include "itkImageDuplicator.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImportImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"

namespace itk
{

template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>
::GaussianSmoothingOnUpdateDisplacementFieldTransform()
{
  this->m_GaussianSmoothingVarianceForTheUpdateField = 3.0;
  this->m_GaussianSmoothingVarianceForTheTotalField = 0.5;
  this->m_GaussianSmoothingTempFieldModifiedTime = 0;

  this->m_GaussianSmoothingSmoother = GaussianSmoothingSmootherType::New();
}

template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>::
~GaussianSmoothingOnUpdateDisplacementFieldTransform()
{
}

template<class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>
::UpdateTransformParameters( DerivativeType & update, ScalarType factor)
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
  if( smoothUpdateField )
    {
    itkDebugMacro( "Smooothing the update field." );

    DisplacementVectorType *updateFieldPointer = reinterpret_cast<DisplacementVectorType *>( update.data_block() );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( updateFieldPointer, numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( displacementField->GetBufferedRegion() );
    importer->SetOrigin( displacementField->GetOrigin() );
    importer->SetSpacing( displacementField->GetSpacing() );
    importer->SetDirection( displacementField->GetDirection() );

    DisplacementFieldPointer field = importer->GetOutput();
    field->Update();
    field->DisconnectPipeline();

    DisplacementFieldPointer smoothedField = this->GaussianSmoothDisplacementField( field, this->m_GaussianSmoothingVarianceForTheUpdateField );

    DerivativeValueType *smoothedFieldPointer = reinterpret_cast<DerivativeValueType *>( smoothedField->GetBufferPointer() );

    if( update.data_block() != smoothedFieldPointer )
      {
      memcpy( update.data_block(), smoothedFieldPointer, sizeof( DisplacementVectorType ) * numberOfPixels );
      }
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
  if( this->m_GaussianSmoothingVarianceForTheTotalField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the total field." );
    smoothTotalField = false;
    }
  if( smoothTotalField )
    {
    itkDebugMacro( "Smooothing the total field." );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( displacementField->GetBufferPointer(), numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( displacementField->GetBufferedRegion() );
    importer->SetOrigin( displacementField->GetOrigin() );
    importer->SetSpacing( displacementField->GetSpacing() );
    importer->SetDirection( displacementField->GetDirection() );

    DisplacementFieldPointer totalField = importer->GetOutput();
    totalField->Update();
    totalField->DisconnectPipeline();

    DisplacementFieldPointer totalSmoothField = this->GaussianSmoothDisplacementField( totalField, this->m_GaussianSmoothingVarianceForTheTotalField );

    DerivativeValueType *displacementFieldPointer = reinterpret_cast<DerivativeValueType *>( displacementField->GetBufferPointer() );
    DerivativeValueType  *totalSmoothFieldPointer = reinterpret_cast<DerivativeValueType *>( totalSmoothField->GetBufferPointer() );

    if( displacementFieldPointer != totalSmoothFieldPointer )
      {
      memcpy( displacementFieldPointer, totalSmoothFieldPointer, sizeof( DisplacementVectorType ) * numberOfPixels );
      }
    }
}

template<class TScalar, unsigned int NDimensions>
typename GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>::DisplacementFieldPointer
GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>
::GaussianSmoothDisplacementField( DisplacementFieldType *field, ScalarType variance )
{
  itkAssertInDebugAndIgnoreInReleaseMacro( variance > 0.0 );

  /* Allocate temp field if new displacement field has been set.
   * We only want to allocate this field if this method is used */
  if( this->GetDisplacementFieldSetTime() > this->m_GaussianSmoothingTempFieldModifiedTime
      || m_GaussianSmoothingTempField.IsNull() )
    {
    this->m_GaussianSmoothingTempFieldModifiedTime = this->GetMTime();
    m_GaussianSmoothingTempField = DisplacementFieldType::New();
    m_GaussianSmoothingTempField->SetSpacing( field->GetSpacing() );
    m_GaussianSmoothingTempField->SetOrigin( field->GetOrigin() );
    m_GaussianSmoothingTempField->SetDirection( field->GetDirection() );
    m_GaussianSmoothingTempField->SetLargestPossibleRegion( field->GetLargestPossibleRegion() );
    m_GaussianSmoothingTempField->SetRequestedRegion( field->GetRequestedRegion() );
    m_GaussianSmoothingTempField->SetBufferedRegion( field->GetBufferedRegion() );
    m_GaussianSmoothingTempField->Allocate();
    }

  typedef typename DisplacementFieldType::PixelType   VectorType;

  typedef typename DisplacementFieldType::PixelContainerPointer PixelContainerPointer;
  // I think we need to keep this as SmartPointer type, to preserve the
  // reference counting so we can assign the swapPtr to the main field and
  // not have to do a memory copy - this happens when image dimensions are odd.
  PixelContainerPointer swapPtr;

  // graft the output field onto the mini-pipeline
  m_GaussianSmoothingSmoother->GraftOutput( m_GaussianSmoothingTempField );

  for( unsigned int j = 0; j < Superclass::Dimension; j++ )
    {
    // smooth along this dimension
    m_GaussianSmoothingOperator.SetDirection( j );
    m_GaussianSmoothingOperator.SetVariance( variance );
    m_GaussianSmoothingOperator.SetMaximumError(0.001 );
    m_GaussianSmoothingOperator.SetMaximumKernelWidth( 256 );
    m_GaussianSmoothingOperator.CreateDirectional();

    // todo: make sure we only smooth within the buffered region
    m_GaussianSmoothingSmoother->SetOperator( m_GaussianSmoothingOperator );
    m_GaussianSmoothingSmoother->SetInput( field );
    try
      {
      m_GaussianSmoothingSmoother->Update();
      }
    catch( ExceptionObject & exc )
      {
      std::string msg("Caught exception: ");
      msg += exc.what();
      itkExceptionMacro( << msg );
      }

    if( j < Superclass::Dimension - 1 )
      {
      // swap the containers
      swapPtr = m_GaussianSmoothingSmoother->GetOutput()->GetPixelContainer();
      m_GaussianSmoothingSmoother->GraftOutput( field );
      // SetPixelContainer does a smartpointer assignment, so the pixel
      // container won't be deleted if field  points to the
      // temporary field upon exiting this method.
      field->SetPixelContainer( swapPtr );
      m_GaussianSmoothingSmoother->Modified();
      }
    }

  this->m_GaussianSmoothingTempField->Graft( this->m_GaussianSmoothingSmoother->GetOutput() );

  //make sure boundary does not move
  ScalarType weight = 1.0;
  if (variance < 0.5)
    {
    weight=1.0 - 1.0 * ( variance / 0.5);
    }
  ScalarType weight2 = 1.0 - weight;
  typedef ImageRegionIteratorWithIndex<DisplacementFieldType> Iterator;
  typename DisplacementFieldType::SizeType size = this->m_GaussianSmoothingTempField->GetLargestPossibleRegion().GetSize();
  Iterator outIter( this->m_GaussianSmoothingTempField, this->m_GaussianSmoothingTempField->GetLargestPossibleRegion() );
  for( outIter.GoToBegin(); !outIter.IsAtEnd(); ++outIter )
  {
    bool onboundary=false;
    typename DisplacementFieldType::IndexType index= outIter.GetIndex();
    for (int i=0; i < Superclass::Dimension; i++)
      {
      if (index[i] < 1 || index[i] >= static_cast<int>( size[i] )-1 )
        {
        onboundary=true;
        }
      }
    if( onboundary )
      {
      VectorType vec;
      vec.Fill(0.0);
      outIter.Set(vec);
      }
    else
      {
      VectorType svec = field->GetPixel( index );
      outIter.Set( svec * weight + outIter.Get() * weight2);
      }
  }

  return m_GaussianSmoothingTempField;
}

template <class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateDisplacementFieldTransform<TScalar, NDimensions>::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  os << indent << "Gaussian smoothing parameters: " << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheUpdateField: " << this->m_GaussianSmoothingVarianceForTheUpdateField
     << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheTotalField: " << this->m_GaussianSmoothingVarianceForTheTotalField
     << std::endl;
}
} // namespace itk

#endif
