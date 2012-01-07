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
#ifndef __itkUnaryFunctorImageFilter_hxx
#define __itkUnaryFunctorImageFilter_hxx

#include "itkUnaryFunctorImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage, class TOutputImage, class TFunction  >
UnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction >
::UnaryFunctorImageFilter()
{
}

/**
 * ThreadedGenerateData Performs the pixel-wise addition
 */
template< class TInputImage, class TOutputImage, class TFunction  >
void
UnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       ThreadIdType threadId)
{
  InputImagePointer  inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput(0);

  // Define the portion of the input to walk for this thread, using
  // the CallCopyOutputRegionToInputRegion method allows for the input
  // and output images to be different dimensions
  InputImageRegionType inputRegionForThread;

  this->CallCopyOutputRegionToInputRegion(inputRegionForThread, outputRegionForThread);

  // Define the iterators
  ImageRegionConstIterator< TInputImage > inputIt(inputPtr, inputRegionForThread);
  ImageRegionIterator< TOutputImage >     outputIt(outputPtr, outputRegionForThread);

  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while ( !inputIt.IsAtEnd() )
    {
    outputIt.Set( m_Functor( inputIt.Get() ) );
    ++inputIt;
    ++outputIt;
    progress.CompletedPixel();  // potential exception thrown here
    }
}
} // end namespace itk

#endif
