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
#ifndef __itkLabelMapOverlayImageFilter_hxx
#define __itkLabelMapOverlayImageFilter_hxx

#include "itkLabelMapOverlayImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

namespace itk {

template< class TAssociate >
void
LabelMapOverlayImageFilterThreader< TAssociate >
::ThreadedExecution( const DomainType & outputRegionForThread,
  const ThreadIdType itkNotUsed(threadId) )
{
  typedef typename AssociateType::OutputImageType  OutputImageType;
  typedef typename AssociateType::LabelMapType     LabelMapType;
  typedef typename AssociateType::FeatureImageType FeatureImageType;
  typedef typename AssociateType::FunctorType      FunctorType;

  OutputImageType * output = this->m_Associate->GetOutput();
  LabelMapType * input = const_cast<LabelMapType *>( this->m_Associate->GetInput() );
  const FeatureImageType * input2 = this->m_Associate->GetFeatureImage();

  FunctorType function;
  function.SetBackgroundValue( input->GetBackgroundValue() );
  function.SetOpacity( this->m_Associate->m_Opacity );

  ImageRegionConstIterator< FeatureImageType > featureIt( input2, outputRegionForThread );
  ImageRegionIterator< OutputImageType > outputIt( output, outputRegionForThread );

  for ( featureIt.GoToBegin(), outputIt.GoToBegin();
        !featureIt.IsAtEnd();
        ++featureIt, ++outputIt )
    {
    outputIt.Set( function( featureIt.Get(), input->GetBackgroundValue() ) );
    }
}


template<class TLabelMap, class TFeatureImage, class TOutputImage>
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::LabelMapOverlayImageFilter()
{
  this->SetNumberOfRequiredInputs(2);
  m_Opacity = 0.5;
  m_Threader = ThreaderType::New();
}


template<class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  LabelMapPointer input = const_cast<LabelMapType *>(this->GetInput());
  if ( !input )
    { return; }
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}


template <class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}


template<class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::GenerateData()
{
  this->AllocateOutputs();

  OutputImageType * output = this->GetOutput();

  this->m_Threader->Execute( this, output->GetRequestedRegion() );

  // and delegate to the superclass implementation to use the thread support for the label objects
  Superclass::GenerateData();
}


template<class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::ThreadedProcessLabelObject( LabelObjectType * labelObject )
{
  OutputImageType * output = this->GetOutput();
  LabelMapType * input = const_cast<LabelMapType *>(this->GetInput());
  const FeatureImageType * input2 = this->GetFeatureImage();

  FunctorType function;
  function.SetBackgroundValue( input->GetBackgroundValue() );
  function.SetOpacity( m_Opacity );

  const typename LabelObjectType::LabelType & label = labelObject->GetLabel();

  // the user want the mask to be the background of the label collection image
  typename LabelObjectType::ConstIndexIterator it( labelObject );
  while( ! it.IsAtEnd() )
    {
    const IndexType idx = it.GetIndex();
    output->SetPixel( idx, function( input2->GetPixel(idx), label ) );
    ++it;
    }

}

template<class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::GenerateOutputInformation()
{
  // this methods is overloaded so that if the output image is a
  // VectorImage then the correct number of components are set.

  Superclass::GenerateOutputInformation();
  OutputImageType* output = this->GetOutput();

  if ( !output )
    {
    return;
    }
  if ( output->GetNumberOfComponentsPerPixel() != 3 )
    {
    output->SetNumberOfComponentsPerPixel( 3 );
    }
}

template<class TLabelMap, class TFeatureImage, class TOutputImage>
void
LabelMapOverlayImageFilter<TLabelMap, TFeatureImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Opacity: " << m_Opacity << std::endl;
}


}// end namespace itk
#endif
