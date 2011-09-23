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
#ifndef __itkMaskedFFTNormalizedCorrelationImageFilter_hxx
#define __itkMaskedFFTNormalizedCorrelationImageFilter_hxx

#include "itkMaskedFFTNormalizedCorrelationImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkForwardFFTImageFilter.h"
#include "itkInverseFFTImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

namespace itk
{
template<class TInputImage, class TOutputImage>
void MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  // Check that the image sizes are the same as their corresponding masks.  If not, exit with an
  // error.
  for (unsigned int i = 0; i < ImageDimension; i++)
  {
    if (this->GetFixedImageInput()->GetLargestPossibleRegion().GetSize()[i]
        != this->GetFixedMaskInput()->GetLargestPossibleRegion().GetSize()[i])
    {
      itkExceptionMacro(<< "The fixed image must be the same size as the fixed mask");
    }
    if (this->GetMovingImageInput()->GetLargestPossibleRegion().GetSize()[i]
        != this->GetMovingMaskInput()->GetLargestPossibleRegion().GetSize()[i])
    {
      itkExceptionMacro(<< "The moving image must be the same size as the moving mask");
    }
  }

  // Cast the images to the Real pixel type.
  typedef CastImageFilter<InputImageType, RealImageType> CastType;
  typename CastType::Pointer fixedImageCaster = CastType::New();
  fixedImageCaster->SetInput(this->GetFixedImageInput());
  fixedImageCaster->Update();
  typename CastType::Pointer movingImageCaster = CastType::New();
  movingImageCaster->SetInput( this->GetMovingImageInput() );
  movingImageCaster->Update();
  typename CastType::Pointer fixedMaskCaster = CastType::New();
  fixedMaskCaster->SetInput(this->GetFixedMaskInput());
  fixedMaskCaster->Update();
  typename CastType::Pointer movingMaskCaster = CastType::New();
  movingMaskCaster->SetInput( this->GetMovingMaskInput() );
  movingMaskCaster->Update();

  // Update needs to be called on the casters before their outputs can be assigned to the image pointers.
  typename RealImageType::Pointer fixedImage = fixedImageCaster->GetOutput();
  typename RealImageType::Pointer movingImage = movingImageCaster->GetOutput();
  typename RealImageType::Pointer fixedMask = fixedMaskCaster->GetOutput();
  typename RealImageType::Pointer movingMask = movingMaskCaster->GetOutput();

  // Ensure that the masks consist of only 0s and 1s.  Anything less
  // than or equal to 0 gets set to 0, and everything else gets set to
  // 1.
  // The fixed and moving images need to be masked for the equations
  // below to work correctly.
  PreprocessImages<RealImageType>(fixedImage,fixedMask);
  PreprocessImages<RealImageType>(movingImage,movingMask);

  // The combinedImageSize is the size resulting from the correlation of the two images.
  RealSizeType combinedImageSize;
  // The FFTImageSize is the closest valid dimension each dimension.
  // The dimension must be divisible by a combination of 2, 3, and 5.
  RealSizeType FFTImageSize;
  for( unsigned int i = 0; i < ImageDimension; i++ )
  {
    combinedImageSize[i] = fixedImage->GetLargestPossibleRegion().GetSize()[i] + fixedImage->GetLargestPossibleRegion().GetSize()[i] - 1;
    FFTImageSize[i] = FindClosestValidDimension( combinedImageSize[i] );
  }

  // Flip the moving images along all dimensions so that the correlation can be more easily handled.
  typedef itk::FlipImageFilter<RealImageType> FlipperType;
  bool bArray[ImageDimension];
  for (unsigned int i = 0; i < ImageDimension; i++)
  {
    bArray[i] = true;
  }
  typename FlipperType::FlipAxesArrayType flipAxes(bArray);
  typename FlipperType::Pointer movingImageRotater = FlipperType::New();
  movingImageRotater->SetFlipAxes(flipAxes);
  movingImageRotater->SetInput(movingImage);
  movingImageRotater->Update();
  typename FlipperType::Pointer movingMaskRotater = FlipperType::New();
  movingMaskRotater->SetFlipAxes(flipAxes);
  movingMaskRotater->SetInput(movingMask);
  movingMaskRotater->Update();

  // Only 6 FFTs are needed.
  // We could potentially calculate two forward FFTs at a time by using odd-even
  // separation.
  FFTImagePointer fixedFFT = CalculateForwardFFT( fixedImage, FFTImageSize );
  FFTImagePointer fixedSquaredFFT = CalculateForwardFFT( ElementProduct<RealImageType>(fixedImage,fixedImage), FFTImageSize );
  FFTImagePointer fixedMaskFFT = CalculateForwardFFT( fixedMask, FFTImageSize );
  FFTImagePointer rotatedMovingFFT = CalculateForwardFFT( movingImageRotater->GetOutput(), FFTImageSize );
  FFTImagePointer rotatedMovingSquaredFFT = CalculateForwardFFT( ElementProduct<RealImageType>(movingImageRotater->GetOutput(),movingImageRotater->GetOutput()), FFTImageSize );
  FFTImagePointer rotatedMovingMaskFFT = CalculateForwardFFT( movingMaskRotater->GetOutput(), FFTImageSize );

  // Only 6 IFFTs are needed.
  // Compute and save these results rather than computing them multiple times.
  RealImagePointer numberOfOverlapPixels = CalculateInverseFFT(ElementProduct<FFTImageType>(fixedMaskFFT,rotatedMovingMaskFFT),combinedImageSize);
  // Ensure that the result is positive.
  ElementRound<RealImageType>(numberOfOverlapPixels);
  ElementPositive<RealImageType>(numberOfOverlapPixels);

  RealImagePointer fixedCumulativeSumImage = CalculateInverseFFT(ElementProduct<FFTImageType>(fixedFFT,rotatedMovingMaskFFT),combinedImageSize);
  RealImagePointer rotatedMovingCumulativeSumImage = CalculateInverseFFT(ElementProduct<FFTImageType>(fixedMaskFFT,rotatedMovingFFT),combinedImageSize);
  RealImagePointer numerator = ElementSubtraction<RealImageType>(
      CalculateInverseFFT(ElementProduct<FFTImageType>(fixedFFT,rotatedMovingFFT),combinedImageSize),
      ElementQuotient<RealImageType>(ElementProduct<RealImageType>(fixedCumulativeSumImage,rotatedMovingCumulativeSumImage),numberOfOverlapPixels));

  RealImagePointer fixedDenom = ElementSubtraction<RealImageType>(
      CalculateInverseFFT(ElementProduct<FFTImageType>(fixedSquaredFFT,rotatedMovingMaskFFT),combinedImageSize),
      ElementQuotient<RealImageType>(ElementProduct<RealImageType>(fixedCumulativeSumImage,fixedCumulativeSumImage),numberOfOverlapPixels));
  // Ensure that the result is positive.
  ElementPositive<RealImageType>(fixedDenom);

  RealImagePointer rotatedMovingDenom = ElementSubtraction<RealImageType>(
      CalculateInverseFFT(ElementProduct<FFTImageType>(fixedMaskFFT,rotatedMovingSquaredFFT),combinedImageSize),
      ElementQuotient<RealImageType>(ElementProduct<RealImageType>(rotatedMovingCumulativeSumImage,rotatedMovingCumulativeSumImage),numberOfOverlapPixels));
  // Ensure that the result is positive.
  ElementPositive<RealImageType>(rotatedMovingDenom);

  RealImagePointer denominator = ElementProduct<RealImageType>(fixedDenom,rotatedMovingDenom);
  ElementSqrt<RealImageType>(denominator);

  RealImagePointer NCC = ElementQuotient<RealImageType>(numerator,denominator);

  // The correlation must be between -1 and 1 by definition.  But
  // numerical errors can cause the values to be large values (for
  // example, when dividing by zero).  So, we loop through the matrix
  // and set to zero all values outside of this range.
  itk::ImageRegionConstIterator<RealImageType> denomIt(denominator,denominator->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<RealImageType> overlapIt(numberOfOverlapPixels,numberOfOverlapPixels->GetLargestPossibleRegion());
  itk::ImageRegionIterator<RealImageType> NCCIt( NCC, NCC->GetLargestPossibleRegion());
  for( denomIt.GoToBegin(), overlapIt.GoToBegin(), NCCIt.GoToBegin(); !NCCIt.IsAtEnd(); ++denomIt, ++overlapIt, ++NCCIt )
  {
    if( denomIt.Get() == 0 || overlapIt.Get() == 0 )
    {
      NCCIt.Set(0.0);
    }
    else if( NCCIt.Get() < -1 )
    {
      NCCIt.Set(-1.0);
    }
    else if( NCCIt.Get() > 1 )
    {
      NCCIt.Set(1.0);
    }
  }

  typedef itk::CastImageFilter<RealImageType,OutputImageType> OutputCastType;
  typename OutputCastType::Pointer outputCaster = OutputCastType::New();
  outputCaster->SetInput( NCC );
  outputCaster->Update();
  this->GetOutput()->Graft( outputCaster->GetOutput() );

  typename CastImageFilter<RealImageType,ULongImageType>::Pointer numberOfOverlapPixelsCaster = CastImageFilter<RealImageType,ULongImageType>::New();
  numberOfOverlapPixelsCaster->SetInput( numberOfOverlapPixels );
  numberOfOverlapPixelsCaster->Update();
  m_NumberOfOverlapPixels = numberOfOverlapPixelsCaster->GetOutput();
}


template< class TInputImage, class TOutputImage >
template< class GenericImageType >
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::PreprocessImages( GenericImageType * inputImage, GenericImageType * inputMask )
{
  // Process the intensity image and mask.
  ImageRegionIterator < GenericImageType > intensityIt(inputImage, inputImage->GetLargestPossibleRegion());
  ImageRegionIterator < GenericImageType > maskIt(inputMask, inputMask->GetLargestPossibleRegion());
  for (intensityIt.GoToBegin(), maskIt.GoToBegin(); !intensityIt.IsAtEnd(); ++intensityIt, ++maskIt)
  {
    if (maskIt.Get() <= 0)
    {
      maskIt.Set(0.0);
      intensityIt.Set(0.0);
    }
    else
    {
      maskIt.Set(1.0);
    }
  }
}

template< class TInputImage, class TOutputImage >
typename Image< std::complex<double>, TOutputImage::ImageDimension >::Pointer
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::CalculateForwardFFT( RealImageType * inputImage, RealSizeType & FFTImageSize )
 {
  RealPixelType constantPixel = 0;
  RealRegionType inputRegion = inputImage->GetLargestPossibleRegion();
  RealRegionType fullRegion;
  fullRegion.SetSize( FFTImageSize );
  fullRegion.SetIndex( inputRegion.GetIndex() );
  typename RealImageType::Pointer paddedImage = RealImageType::New();
  paddedImage->SetRegions( fullRegion );
  paddedImage->Allocate();
  paddedImage->FillBuffer( constantPixel );

  // Pad the image with zeros on the right and
  // bottom so that the resulting matrices have the size given by
  // FFTImageSize.
  RealIndexType outputIndex = inputRegion.GetIndex();
  RealRegionType outputRegion = inputRegion;
  outputRegion.SetIndex( outputIndex );

  itk::ImageRegionConstIterator< RealImageType > inputIt( inputImage, inputRegion);
  itk::ImageRegionIterator< RealImageType > outputIt( paddedImage, outputRegion );
  for( inputIt.GoToBegin(), outputIt.GoToBegin(); !outputIt.IsAtEnd(); ++inputIt, ++outputIt )
  {
    outputIt.Set( inputIt.Get() );
  }

  typedef itk::ForwardFFTImageFilter<RealImageType, FFTImageType > FFTFilterType;
  typename FFTFilterType::Pointer FFTFilter = FFTFilterType::New();
  FFTFilter->SetInput( paddedImage );
  FFTFilter->Update();

  return FFTFilter->GetOutput();
 }

template< class TInputImage, class TOutputImage >
typename Image< double, TOutputImage::ImageDimension>::Pointer
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::CalculateInverseFFT(FFTImageType * inputImage, RealSizeType & combinedImageSize )
 {
  // The inverse Fourier transform normalizes by the number of pixels in the Fourier image.
  // It also converts the image from complex (with small imaginary values since
  // the input to the original FFTs was real) to real.
  typedef itk::InverseFFTImageFilter<FFTImageType, RealImageType> FFTFilterType;
  typename FFTFilterType::Pointer FFTFilter = FFTFilterType::New();
  FFTFilter->SetInput( inputImage );

  // Extract the relevant part out of the image.
  // The input FFT image may be bigger than the desired output image
  // because specific sizes are required for the FFT calculation.
  typename RealImageType::RegionType imageRegion;
  typename RealImageType::IndexType imageIndex;
  imageIndex.Fill(0);
  imageRegion.SetIndex(imageIndex);
  imageRegion.SetSize(combinedImageSize);
  typedef itk::RegionOfInterestImageFilter<RealImageType,RealImageType> ExtractType;
  typename ExtractType::Pointer extracter = ExtractType::New();
  extracter->SetInput(FFTFilter->GetOutput());
  extracter->SetRegionOfInterest(imageRegion);
  extracter->Update();

  return extracter->GetOutput();
 }

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
typename GenericImageType::Pointer
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementProduct( GenericImageType * inputImage1, GenericImageType * inputImage2 )
{
  typedef itk::MultiplyImageFilter<GenericImageType,GenericImageType,GenericImageType> MultiplyType;
  typename MultiplyType::Pointer multiplier = MultiplyType::New();
  multiplier->SetInput1( inputImage1 );
  multiplier->SetInput2( inputImage2 );
  multiplier->Update();
  return multiplier->GetOutput();
}

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
typename GenericImageType::Pointer
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementQuotient( GenericImageType * inputImage1, GenericImageType * inputImage2 )
{
  typedef itk::DivideImageFilter<GenericImageType,GenericImageType,GenericImageType> DivideType;
  typename DivideType::Pointer divider = DivideType::New();
  divider->SetInput1( inputImage1 );
  divider->SetInput2( inputImage2 );
  divider->Update();
  return divider->GetOutput();
}

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
typename GenericImageType::Pointer
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementSubtraction( GenericImageType * inputImage1, GenericImageType * inputImage2 )
 {
  typedef itk::SubtractImageFilter<GenericImageType,GenericImageType,GenericImageType> SubtractType;
  typename SubtractType::Pointer subtracter = SubtractType::New();
  subtracter->SetInput1( inputImage1 );
  subtracter->SetInput2( inputImage2 );
  subtracter->Update();
  return subtracter->GetOutput();
 }

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementPositive( GenericImageType * inputImage )
 {
  typedef typename GenericImageType::PixelType PixelType;
  itk::ImageRegionIterator<GenericImageType> imageIt( inputImage,inputImage->GetLargestPossibleRegion() );
  for( imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt )
  {
    imageIt.Set( vnl_math_max(imageIt.Get(),PixelType(0)));
  }
 }

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementRound( GenericImageType * inputImage )
 {
  itk::ImageRegionIterator<GenericImageType> imageIt( inputImage,inputImage->GetLargestPossibleRegion() );
  for( imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt )
  {
    imageIt.Set( vnl_math_rnd(imageIt.Get()));
  }
 }

template< class TInputImage, class TOutputImage >
template< class GenericImageType >
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::ElementSqrt( GenericImageType * inputImage )
 {
  itk::ImageRegionIterator<GenericImageType> imageIt( inputImage,inputImage->GetLargestPossibleRegion() );
  for( imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt )
  {
    imageIt.Set( vcl_sqrt(imageIt.Get()));
  }
 }

template< class TInputImage, class TOutputImage >
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

template <class TInputImage, class TOutputImage>
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  this->GetFixedImageInput()->SetRequestedRegionToLargestPossibleRegion();
  this->GetMovingImageInput()->SetRequestedRegionToLargestPossibleRegion();
  this->GetFixedMaskInput()->SetRequestedRegionToLargestPossibleRegion();
  this->GetMovingMaskInput()->SetRequestedRegionToLargestPossibleRegion();
}

template <class TInputImage, class TOutputImage>
void
MaskedFFTNormalizedCorrelationImageFilter<TInputImage,TOutputImage>
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

// This function factorizes the image size uses factors of 2, 3, and
// 5.  After this factorization, if there are any remaining values,
// the function returns this value.
template <class TInputImage, class TOutputImage>
int
MaskedFFTNormalizedCorrelationImageFilter<TInputImage,TOutputImage>
::FactorizeNumber( int n )
{
  int ifac = 2;
  // This loop is just a convenient way of ensuring that ifac assumes
  // values of 2, 3, and 5 and then quits.  These are the only factors
  // that are valid for the FFT calculation.
  for (int offset = 1; offset <= 3; offset++)
    {
    // Using the given factor, factor the image continuously until it
    // can no longer be factored with this value.
    for(; n % ifac == 0;)
      {
      n /= ifac;
      }
    ifac += offset;
    }
  return n;
}

// Find the closest valid dimension above the desired dimension.  This
// will be a combination of 2s, 3s, and 5s.
template <class TInputImage, class TOutputImage>
int
MaskedFFTNormalizedCorrelationImageFilter<TInputImage,TOutputImage>
::FindClosestValidDimension( int n )
{
  // Incrementally add 1 to the size until
  // we reach a size that can be properly factored.
  int newNumber = n;
  int result = 0;
  newNumber -= 1;
  while( result!=1 )
    {
    newNumber += 1;
    result = FactorizeNumber(newNumber);
    }
  return newNumber;
}

} // end namespace itk

#endif
