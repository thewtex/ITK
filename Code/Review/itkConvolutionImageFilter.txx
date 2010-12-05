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

#ifndef __itkConvolutionImageFilter_txx
#define __itkConvolutionImageFilter_txx

#include "itkConvolutionImageFilter.h"

#include "itkImageBase.h"
#include "itkImageKernelOperator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkProgressReporter.h"

#include "vnl/vnl_math.h"

namespace itk
{
template< class TInputImage, class TKernelImage, class TComputationPixel, class TOutputImage >
ConvolutionImageFilter< TInputImage, TKernelImage, TComputationPixel, TOutputImage >
::ConvolutionImageFilter()
{
  this->SetNumberOfRequiredInputs(2);
  m_Normalize = false;
}

template< class TInputImage, class TKernelImage, class TComputationPixel, class TOutputImage >
ConvolutionImageFilter< TInputImage, TKernelImage, TComputationPixel, TOutputImage >
::~ConvolutionImageFilter()
{}

template< class TInputImage, class TKernelImage, class TComputationPixel, class TOutputImage >
void
ConvolutionImageFilter< TInputImage, TKernelImage, TComputationPixel, TOutputImage >
::ThreadedGenerateData(const OutputRegionType & outputRegionForThread, int threadId)
{
  // setup the progress reporter
  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

  typename KernelImageType::Pointer kernel = const_cast< KernelImageType * >(
      static_cast< const KernelImageType * >( this->ProcessObject::GetInput(1) ) );
  if( kernel.GetPointer() == NULL )
    {
    return;
    }

  typedef ConstNeighborhoodIterator< InputImageType >   NeighborhoodIteratorType;
  typedef typename NeighborhoodIteratorType::RadiusType RadiusType;
  typedef typename RadiusType::SizeValueType            SizeValueType;
  RadiusType radius;
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    radius[i] = (kernel->GetLargestPossibleRegion().GetSize()[i] - 1) / 2;
    }

  KernelPixelType scalingFactor = NumericTraits< KernelPixelType >::One;
  if ( this->GetNormalize() )
    {
    KernelPixelType                             sum = NumericTraits< KernelPixelType >::Zero;
    ImageRegionConstIterator< KernelImageType > It( kernel,
                                                    kernel->GetLargestPossibleRegion() );
    for ( It.GoToBegin(); !It.IsAtEnd(); ++It )
      {
      sum += It.Get();
      }
    if ( sum != NumericTraits< KernelPixelType >::One
         || sum != NumericTraits< KernelPixelType >::Zero )
      {
      scalingFactor = NumericTraits< KernelPixelType >::One * 1./ sum;
      }
    }

  typedef typename NeighborhoodAlgorithm
  ::ImageBoundaryFacesCalculator< InputImageType > FaceCalculatorType;
  FaceCalculatorType faceCalculator;

  NeighborhoodInnerProduct< InputImageType, KernelPixelType, ComputationPixelType > innerProduct;

  ImageKernelOperator< KernelPixelType, ImageDimension > imageKernelOperator;
  imageKernelOperator.SetImageKernel(kernel);
  imageKernelOperator.CreateToRadius(radius);

  typename FaceCalculatorType::FaceListType faceList = faceCalculator(
    this->GetInput(0), outputRegionForThread, radius);
  typename FaceCalculatorType::FaceListType::iterator fit;

  for ( fit = faceList.begin(); fit != faceList.end(); ++fit )
    {
    NeighborhoodIteratorType               inIt(radius, this->GetInput(0), *fit);
    ImageRegionIterator< OutputImageType > outIt(this->GetOutput(), *fit);

    for ( inIt.GoToBegin(), outIt.GoToBegin(); !inIt.IsAtEnd();
          ++inIt, ++outIt )
      {
      outIt.Set( static_cast< OutputPixelType >(
                   scalingFactor * innerProduct(inIt, imageKernelOperator) ) );
      progress.CompletedPixel();
      }
    }
}

/**
 * ConvolutionImageFilter needs a smaller 2nd input (the image kernel)
 * requested region than output requested region.  As such,  this filter
 * needs to provide an implementation for GenerateInputRequestedRegion() in
 * order to inform the pipeline execution model.
 *
 * \sa ProcessObject::GenerateInputRequestedRegion()
 */
template< class TInputImage, class TKernelImage, class TComputationPixel, class TOutputImage >
void
ConvolutionImageFilter< TInputImage, TKernelImage, TComputationPixel, TOutputImage >
::GenerateInputRequestedRegion()
{
  // Simply copy the GenerateInputRequestedRegion() function and
  // deal with the image kernel as a special case.
  for ( unsigned int idx = 0; idx < 2; ++idx )
    {
    if ( this->GetInput(idx) )
      {
      // Check whether the input is an image of the appropriate
      // dimension (use ProcessObject's version of the GetInput()
      // method since it returns the input as a pointer to a
      // DataObject as opposed to the subclass version which
      // static_casts the input to an TInputImage).
      typedef ImageBase< ImageDimension > ImageBaseType;
      typename ImageBaseType::ConstPointer constInput =
        dynamic_cast< ImageBaseType const * >(
          this->ProcessObject::GetInput(idx) );

      if ( constInput.IsNull() )
        {
        itkExceptionMacro("Input image " << idx
                                         << " not correctly specified.");
        }

      typename InputImageType::RegionType inputRegion;
      if ( idx == 0 )
        {
        // Input is an image, cast away the constness so we can set
        // the requested region.
        typename InputImageType::Pointer input =
          const_cast< TInputImage * >( this->GetInput(idx) );
        Superclass::CallCopyOutputRegionToInputRegion( inputRegion,
                                                       this->GetOutput()->GetRequestedRegion() );
        input->SetRequestedRegion(inputRegion);
        }
      else  // the input is the image kernel
        {
        typename KernelImageType::Pointer input =
          const_cast< TKernelImage * >( static_cast< const TKernelImage *>( this->ProcessObject::GetInput(idx) ) );
        typename KernelImageType::RegionType::SizeType  inputSize;
        typename KernelImageType::RegionType::IndexType inputIndex;
        inputSize = this->GetInput(idx)->GetLargestPossibleRegion().GetSize();
        inputIndex = this->GetInput(idx)->GetLargestPossibleRegion().GetIndex();
        inputRegion.SetSize(inputSize);
        inputRegion.SetIndex(inputIndex);
        input->SetRequestedRegion(inputRegion);
        }
      }
    }
}

template< class TInputImage, class TKernelImage, class TComputationPixel, class TOutputImage >
void
ConvolutionImageFilter< TInputImage, TKernelImage, TComputationPixel, TOutputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Normalize: "  << m_Normalize << std::endl;
  //  NOT REALLY MEMBER DATA. Need to fool PrintSelf check
  //  os << indent << "ImageKernel: "  << m_ImageKernel << std::e0ndl;
}

} // end namespace itk
#endif
