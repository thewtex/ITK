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
#ifndef __itkFFTHalfToFullImageFilter_hxx
#define __itkFFTHalfToFullImageFilter_hxx

#include "itkFFTHalfToFullImageFilter.h"

#include "itkImageAlgorithm.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressReporter.h"

namespace itk
{

template< class TInputImage >
void
FFTHalfToFullImageFilter< TInputImage >
::GenerateOutputInformation()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // Get pointers to the input and output
  typename InputImageType::ConstPointer inputPtr = this->GetInput();
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // This is all based on
  // itk::InverseFFTImageFilter::GenerateOutputInformation()
  const typename InputImageType::SizeType &   inputSize =
    inputPtr->GetLargestPossibleRegion().GetSize();
  const typename InputImageType::IndexType &  inputStartIndex =
    inputPtr->GetLargestPossibleRegion().GetIndex();

  typename OutputImageType::SizeType outputSize;
  typename OutputImageType::IndexType outputStartIndex;

  typedef typename InputImageType::SizeType::SizeValueType SizeScalarType;

  for ( unsigned int i = 0; i < OutputImageType::ImageDimension; i++ )
    {
    outputSize[i] = inputSize[i];
    outputStartIndex[i] = inputStartIndex[i];
    }
  outputSize[0] = ( inputSize[0] - 1 ) * 2;
  if ( this->GetActualXDimensionIsOdd() )
    {
    outputSize[0]++;
    }


  typename OutputImageType::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( outputSize );
  outputLargestPossibleRegion.SetIndex( outputStartIndex );

  outputPtr->SetLargestPossibleRegion( outputLargestPossibleRegion );
}

template< class TInputImage >
void
FFTHalfToFullImageFilter< TInputImage >
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();

  // Get pointers to the input and output
  typename InputImageType::Pointer inputPtr  =
    const_cast< InputImageType * >( this->GetInput() );
  if ( inputPtr )
    {
    inputPtr->SetRequestedRegionToLargestPossibleRegion();
    }
}

template< class TInputImage >
void
FFTHalfToFullImageFilter< TInputImage >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       ThreadIdType threadId)
{
  // Get pointers to the input and output.
  typename InputImageType::ConstPointer inputPtr = this->GetInput();
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  InputImageRegionType inputRegion = inputPtr->GetLargestPossibleRegion();
  InputImageIndexType  inputRegionIndex = inputRegion.GetIndex();
  InputImageSizeType   inputRegionSize = inputRegion.GetSize();
  InputImageIndexType  inputRegionMaximumIndex = inputRegionIndex + inputRegionSize;

  // Copy the non-reflected region.
  OutputImageRegionType copyRegion( outputRegionForThread );
  bool copy = copyRegion.Crop( inputRegion );
  float initialProgress = 0.0f;
  if ( copy )
    {
    ImageAlgorithm::Copy( inputPtr.GetPointer(), outputPtr.GetPointer(),
                          copyRegion, copyRegion );
    initialProgress = static_cast< float >( copyRegion.GetNumberOfPixels() ) /
      static_cast< float >( outputRegionForThread.GetNumberOfPixels() );
    }

  // Set up the ProgressReporter.
  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels(),
                             100, initialProgress );

  // Now copy the redundant complex conjugate region, if there is one
  // in this thread's output region.
  OutputImageIndexType outputRegionIndex = outputRegionForThread.GetIndex();
  OutputImageSizeType  outputRegionSize = outputRegionForThread.GetSize();
  OutputImageIndexType outputRegionMaximumIndex = outputRegionIndex + outputRegionSize;

  if ( outputRegionMaximumIndex[0] > inputRegionMaximumIndex[0] )
    {
    OutputImageIndexType conjugateIndex( outputRegionIndex );
    conjugateIndex[0] = std::max(outputRegionIndex[0], inputRegionMaximumIndex[0] );
    OutputImageSizeType conjugateSize( outputRegionSize );
    conjugateSize[0] = outputRegionMaximumIndex[0] - conjugateIndex[0];
    OutputImageRegionType conjugateRegion( conjugateIndex, conjugateSize );

    ImageRegionIteratorWithIndex< OutputImageType > oIt( outputPtr, conjugateRegion );
    for (oIt.GoToBegin(); !oIt.IsAtEnd(); ++oIt)
      {
      OutputImageIndexType conjugateIndex = oIt.GetIndex();

      // Flip the indices in each dimension.
      OutputImageIndexType index( conjugateIndex );
      for (unsigned int i = 0; i < ImageDimension; ++i)
        {
        if ( conjugateIndex[i] != 0 )
          {
          OutputImageRegionType outputLargestPossibleRegion =
            outputPtr->GetLargestPossibleRegion();
          OutputImageSizeType outputLargestPossibleSize =
            outputLargestPossibleRegion.GetSize();
          index[i] = outputLargestPossibleSize[i] - conjugateIndex[i];
          }
        }

      oIt.Set( std::conj( inputPtr->GetPixel( index ) ) );
      progress.CompletedPixel();
      }
    }
}

} // end namespace itk

#endif // __itkFFTHalfToFullImageFilter_hxx
