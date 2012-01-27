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
#ifndef __itkInvertDisplacementFieldImageFilter_hxx
#define __itkInvertDisplacementFieldImageFilter_hxx

#include "itkInvertDisplacementFieldImageFilter.h"

#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkStatisticsImageFilter.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkVectorMagnitudeImageFilter.h"

namespace itk
{

/*
 * InvertDisplacementFieldImageFilter class definitions
 */
template<class TInputImage, class TOutputImage>
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::InvertDisplacementFieldImageFilter() :
 m_MaximumNumberOfIterations( 20 ),
 m_MaxErrorToleranceThreshold( 0.1 ),
 m_MeanErrorToleranceThreshold( 0.001 )
{
  this->SetNumberOfRequiredInputs( 1 );

  typedef VectorLinearInterpolateImageFunction <InputFieldType, RealType> DefaultInterpolatorType;
  typename DefaultInterpolatorType::Pointer interpolator = DefaultInterpolatorType::New();
  this->m_Interpolator = interpolator;

  this->m_ComposedField = DisplacementFieldType::New();
}

template<class TInputImage, class TOutputImage>
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::~InvertDisplacementFieldImageFilter()
{
}

template<class TInputImage, class TOutputImage>
void
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::SetInterpolator( InterpolatorType *interpolator )
{
  itkDebugMacro( "setting Interpolator to " << interpolator );
  if ( this->m_Interpolator != interpolator )
    {
    this->m_Interpolator = interpolator;
    this->Modified();
    if( !this->GetDisplacementField() )
      {
      this->m_Interpolator->SetInputImage( this->GetInput( 0 ) );
      }
    }
}

template<class TInputImage, class TOutputImage>
void
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputs();
  VectorType zeroVector( 0.0 );
  typename DisplacementFieldType::ConstPointer displacementField = this->GetInput();
  typename InverseDisplacementFieldType::Pointer inverseDisplacementField = this->GetOutput();
  inverseDisplacementField->FillBuffer( zeroVector );
  this->m_NormalizationFactor = 1.0;
  for( unsigned int d = 0; d < ImageDimension; d++ )
    {
    this->m_DisplacementFieldSpacing[d] = displacementField->GetSpacing()[d];
    this->m_NormalizationFactor /= this->m_DisplacementFieldSpacing[d];
    }
  this->m_MaxErrorNorm = NumericTraits<RealType>::max();
  this->m_MeanErrorNorm = NumericTraits<RealType>::max();
  unsigned int iteration = 0;
  unsigned int mymaxiter = 20;
  typedef Image<RealType,ImageDimension>  ImageType;
  typename ImageType::Pointer magImage = ImageType::New();
  magImage->SetLargestPossibleRegion( displacementField->GetLargestPossibleRegion() );
  magImage->SetBufferedRegion( displacementField->GetLargestPossibleRegion().GetSize() );
  magImage->SetSpacing( displacementField->GetSpacing() );
  magImage->SetOrigin( displacementField->GetOrigin() );
  magImage->SetDirection( displacementField->GetDirection() );
  magImage->Allocate();

  typedef typename DisplacementFieldType::Pointer DisplacementFieldPointer;
  typedef typename DisplacementFieldType::PixelType VectorType;
  typedef typename DisplacementFieldType::IndexType IndexType;
  typedef typename VectorType::ValueType           ScalarType;
  typedef ImageRegionConstIteratorWithIndex<DisplacementFieldType> CIterator;
  typedef ImageRegionIteratorWithIndex<DisplacementFieldType> Iterator;

  typedef typename DisplacementFieldType::SizeType SizeType;
  SizeType size = displacementField->GetLargestPossibleRegion().GetSize();

  typename ImageType::SpacingType spacing = displacementField->GetSpacing();
  RealType subpix = 0.0;
  unsigned long npix = 1;
  for (int j = 0; j < ImageDimension; j++)
  {
    npix *= displacementField->GetLargestPossibleRegion().GetSize()[j];
  }
  subpix = pow((RealType)ImageDimension,(RealType)ImageDimension) * 0.5;

  RealType max = NumericTraits<RealType>::Zero;
  CIterator iter( displacementField, displacementField->GetLargestPossibleRegion() );
  RealType scale = (1.)/max;
  if (scale > 1.) scale = 1.0;
  Iterator vfIter( inverseDisplacementField, inverseDisplacementField->GetLargestPossibleRegion() );

  unsigned int ct = 0;
  RealType stepl;
  RealType epsilon = 1;
  while ( this->m_MaxErrorNorm > this->m_MaxErrorToleranceThreshold &&
          ct < this->m_MaximumNumberOfIterations &&
          this->m_MeanErrorNorm > m_MeanErrorToleranceThreshold )
    {
    this->m_MeanErrorNorm = NumericTraits<RealType>::Zero;
    //this field says what position the eulerian field should contain in the E domain
    typedef ComposeDisplacementFieldsImageFilter<DisplacementFieldType> ComposerType;
    typename ComposerType::Pointer composer = ComposerType::New();
    composer->SetDisplacementField( displacementField );
    composer->SetWarpingField( inverseDisplacementField );
    this->m_ComposedField = composer->GetOutput();
    this->m_ComposedField->Update();
    this->m_ComposedField->DisconnectPipeline();
    this->m_MaxErrorNorm = NumericTraits<RealType>::Zero;
    for(  vfIter.GoToBegin(); !vfIter.IsAtEnd(); ++vfIter )
      {
      IndexType  index = vfIter.GetIndex();
      VectorType  update = this->m_ComposedField->GetPixel(index);
      RealType mag = NumericTraits<RealType>::Zero;
      for ( int j = 0; j < ImageDimension; j++ )
        {
        update[j] *= (-1.0);
        mag += ( update[j]/spacing[j] ) * ( update[j]/spacing[j] );
        }
      mag = sqrt( mag );
      this->m_MeanErrorNorm += mag;
      if ( mag > this->m_MaxErrorNorm ) this->m_MaxErrorNorm = mag;
      this->m_ComposedField->SetPixel( index, update );
      magImage->SetPixel( index ,mag );
      }
    this->m_MeanErrorNorm /= (RealType) npix;
    if (ct == 0) epsilon = 0.75;
    else epsilon = 0.5;
    stepl = this->m_MaxErrorNorm*epsilon;
    for(  vfIter.GoToBegin(); !vfIter.IsAtEnd(); ++vfIter )
      {
      RealType val = magImage->GetPixel(vfIter.GetIndex() );
      VectorType update = this->m_ComposedField->GetPixel(vfIter.GetIndex() );
      if (val > stepl) update = update * ( stepl/val );
      VectorType upd = vfIter.Get() + update * epsilon;
      vfIter.Set(upd);
      }
    ct++;
    }//end-while

  return;
}

template<class TInputImage, class TOutputImage>
void
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData( const RegionType & region, ThreadIdType itkNotUsed( threadId ) )
{
  ImageRegionIterator<DisplacementFieldType> ItE( this->m_ComposedField, region );
  typename DisplacementFieldType::SpacingType spacing = this->m_ComposedField->GetSpacing();
  if( this->m_DoThreadedEstimateInverse )
    {
    ImageRegionIterator<DisplacementFieldType> ItI( this->GetOutput(), region );
    for( ItI.GoToBegin(), ItE.GoToBegin(); !ItI.IsAtEnd(); ++ItI, ++ItE )
      {
      VectorType update = -ItE.Get();
      RealType updateNorm = NumericTraits<RealType>::Zero;
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        updateNorm += ( (update[i]/spacing[i])*(update[i]/spacing[i]) );
      updateNorm = sqrt(updateNorm);
      if( updateNorm > this->m_Epsilon * this->m_MaxErrorNorm )
        {
        update *= ( this->m_Epsilon * this->m_MaxErrorNorm / updateNorm   );
        }
      ItI.Set( ItI.Get() + update * this->m_Epsilon );
      }
    }
  else
    {
    for( ItE.GoToBegin(); !ItE.IsAtEnd(); ++ItE )
      {
      VectorType displacement = ItE.Get();
      for( unsigned int d = 0; d < ImageDimension; d++ )
        {
        displacement[d] /= this->m_DisplacementFieldSpacing[d];
        }
      ItE.Set( displacement );
      }
    }
}

template<class TInputImage, class TOutputImage>
void
InvertDisplacementFieldImageFilter<TInputImage, TOutputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << "Interpolator:" << std::endl;
  this->m_Interpolator->Print( os, indent );

  os << "Maximum number of iterations: " << this->m_MaximumNumberOfIterations << std::endl;
  os << "Max error tolerance threshold: " << this->m_MaxErrorToleranceThreshold << std::endl;
  os << "Mean error tolerance threshold: " << this->m_MeanErrorToleranceThreshold << std::endl;
}

}  //end namespace itk

#endif
