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
#ifndef __itkBinaryFunctorWithIndexImageFilter_hxx
#define __itkBinaryFunctorWithIndexImageFilter_hxx

#include "itkBinaryFunctorWithIndexImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage1, class TInputImage2,
          class TOutputImage, class TFunction  >
BinaryFunctorWithIndexImageFilter< TInputImage1, TInputImage2, TOutputImage, TFunction >
::BinaryFunctorWithIndexImageFilter()
{
}

/**
 * ThreadedGenerateData Performs the pixel-wise addition
 */
template< class TInputImage1, class TInputImage2, class TOutputImage, class TFunction  >
void
BinaryFunctorWithIndexImageFilter< TInputImage1, TInputImage2, TOutputImage, TFunction >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       ThreadIdType threadId)
{
  // We use dynamic_cast since inputs are stored as DataObjects.  The
  // ImageToImageFilter::GetInput(int) always returns a pointer to a
  // TInputImage1 so it cannot be used for the second input.
  Input1ImagePointer inputPtr1 =
    dynamic_cast< const TInputImage1 * >( ProcessObject::GetInput(0) );
  Input2ImagePointer inputPtr2 =
    dynamic_cast< const TInputImage2 * >( ProcessObject::GetInput(1) );
  OutputImagePointer outputPtr = this->GetOutput(0);

  if( inputPtr1 && inputPtr2 )
    {
    ImageRegionConstIteratorWithIndex< TInputImage1 > inputIt1(inputPtr1,
                                                               outputRegionForThread);
    ImageRegionConstIterator< TInputImage2 > inputIt2(inputPtr2, outputRegionForThread);

    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);

    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt1.GoToBegin();
    inputIt2.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt1.IsAtEnd() )
      {
      typename TInputImage1::IndexType index = inputIt1.GetIndex();
      outputIt.Set( m_Functor( index, inputIt1.Get(), inputIt2.Get() ) );
      ++inputIt2;
      ++inputIt1;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else if( inputPtr1 )
    {
    ImageRegionConstIteratorWithIndex< TInputImage1 > inputIt1(inputPtr1,
                                                               outputRegionForThread);
    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);
    const Input2ImagePixelType & input2Value = this->GetConstant2();
    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt1.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt1.IsAtEnd() )
      {
      typename TInputImage1::IndexType index = inputIt1.GetIndex();
      outputIt.Set( m_Functor( index, inputIt1.Get(), input2Value ) );
      ++inputIt1;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else if( inputPtr2 )
    {
    ImageRegionConstIteratorWithIndex< TInputImage2 > inputIt2(inputPtr2,
                                                               outputRegionForThread);
    ImageRegionIterator< TOutputImage > outputIt(outputPtr, outputRegionForThread);
    const Input1ImagePixelType & input1Value = this->GetConstant1();
    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    inputIt2.GoToBegin();
    outputIt.GoToBegin();

    while ( !inputIt2.IsAtEnd() )
      {
      typename TInputImage2::IndexType index = inputIt2.GetIndex();
      outputIt.Set( m_Functor( index, input1Value, inputIt2.Get() ) );
      ++inputIt2;
      ++outputIt;
      progress.CompletedPixel(); // potential exception thrown here
      }
    }
  else
    {
    itkGenericExceptionMacro(<<"At most one of the inputs can be a constant.");
    }
}
} // end namespace itk

#endif
