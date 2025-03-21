/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkVnlRealToHalfHermitianForwardFFTImageFilter_hxx
#define itkVnlRealToHalfHermitianForwardFFTImageFilter_hxx

#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressReporter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
void
VnlRealToHalfHermitianForwardFFTImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  // Get pointers to the input and output.
  const typename InputImageType::ConstPointer inputPtr = this->GetInput();
  const typename OutputImageType::Pointer     outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // We don't have a nice progress to report, but at least this simple line
  // reports the beginning and the end of the process.
  const ProgressReporter progress(this, 0, 1);

  const InputSizeType inputSize = inputPtr->GetLargestPossibleRegion().GetSize();

  outputPtr->SetBufferedRegion(outputPtr->GetRequestedRegion());
  outputPtr->Allocate();

  unsigned int vectorSize = 1;
  for (unsigned int i = 0; i < ImageDimension; ++i)
  {
    if (!VnlFFTCommon::IsDimensionSizeLegal(inputSize[i]))
    {
      itkExceptionMacro("Cannot compute FFT of image with size "
                        << inputSize << ". VnlRealToHalfHermitianForwardFFTImageFilter operates "
                        << "only on images whose size in each dimension has a prime "
                        << "factorization consisting of only 2s, 3s, or 5s.");
    }
    vectorSize *= inputSize[i];
  }

  const InputPixelType * in = inputPtr->GetBufferPointer();
  SignalVectorType       signal(vectorSize);
  for (unsigned int i = 0; i < vectorSize; ++i)
  {
    signal[i] = in[i];
  }

  // call the proper transform, based on compile type template parameter
  VnlFFTCommon::VnlFFTTransform<InputImageType> vnlfft(inputSize);
  vnlfft.transform(signal.data_block(), -1);

  // Copy the VNL output back to the ITK image.
  for (ImageRegionIteratorWithIndex<TOutputImage> oIt(outputPtr, outputPtr->GetLargestPossibleRegion()); !oIt.IsAtEnd();
       ++oIt)
  {
    const typename OutputImageType::IndexType       index = oIt.GetIndex();
    const typename OutputImageType::OffsetValueType offset = inputPtr->ComputeOffset(index);
    oIt.Set(signal[offset]);
  }
}

template <typename TInputImage, typename TOutputImage>
SizeValueType
VnlRealToHalfHermitianForwardFFTImageFilter<TInputImage, TOutputImage>::GetSizeGreatestPrimeFactor() const
{
  return VnlFFTCommon::GREATEST_PRIME_FACTOR;
}


} // namespace itk

#endif
