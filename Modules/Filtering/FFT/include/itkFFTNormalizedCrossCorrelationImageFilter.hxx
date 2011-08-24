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
#ifndef __itkFFTNormalizedCrossCorrelationImageFilter_hxx
#define __itkFFTNormalizedCrossCorrelationImageFilter_hxx

#include "itkFFTNormalizedCrossCorrelationImageFilter.h"
#include "itkVnlFFTNormalizedCrossCorrelationImageFilter.h"

namespace itk
{

template< typename TSelfPointer, class TInputImage, class TOutputImage, typename TPixel >
struct DispatchFFTNCC_New
{
  static TSelfPointer Apply()
  {
    return VnlFFTNormalizedCrossCorrelationImageFilter< TInputImage, TOutputImage >
    ::New().GetPointer();
  }
};

template< class TInputImage, class TOutputImage >
typename FFTNormalizedCrossCorrelationImageFilter< TInputImage, TOutputImage>::Pointer
FFTNormalizedCrossCorrelationImageFilter< TInputImage, TOutputImage >
::New(void)
{
  Pointer smartPtr = ::itk::ObjectFactory< Self >::Create();

  if( smartPtr.IsNull() )
    {
    smartPtr = DispatchFFTNCC_New< Pointer, TInputImage, TOutputImage,typename NumericTraits< OutputPixelType >::ValueType >
    ::Apply();
    }

  return smartPtr;
}


template <class TInputImage, class TOutputImage>
void
FFTNormalizedCrossCorrelationImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  this->GetFixedImageInput()->SetRequestedRegionToLargestPossibleRegion();
  this->GetMovingImageInput()->SetRequestedRegionToLargestPossibleRegion();
}

template <class TInputImage, class TOutputImage>
void
FFTNormalizedCrossCorrelationImageFilter<TInputImage,TOutputImage>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  InputImageConstPointer fixedImage  = this->GetFixedImageInput();
  InputImageConstPointer movingImage = this->GetMovingImageInput();
  OutputImagePointer     output = this->GetOutput();

  // Compute the size of the output image.
  typename OutputImageType::RegionType region;
  typename OutputImageType::SizeType size;
  for( unsigned int i = 0; i < ImageDimension; ++i )
  {
    size[i] = fixedImage->GetLargestPossibleRegion().GetSize()[i] + movingImage->GetLargestPossibleRegion().GetSize()[i] - 1;
  }
  region.SetSize(size);
  region.SetIndex( fixedImage->GetLargestPossibleRegion().GetIndex() );

  output->SetLargestPossibleRegion(region);
}


} // end namespace itk

#endif
