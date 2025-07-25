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
#ifndef itkShotNoiseImageFilter_hxx
#define itkShotNoiseImageFilter_hxx

#include "itkBitCast.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkImageScanlineIterator.h"
#include "itkTotalProgressReporter.h"
#include "itkNormalVariateGenerator.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
ShotNoiseImageFilter<TInputImage, TOutputImage>::ShotNoiseImageFilter()

{
  this->DynamicMultiThreadingOff();
  this->ThreaderUpdateProgressOff();
}

template <class TInputImage, class TOutputImage>
void
ShotNoiseImageFilter<TInputImage, TOutputImage>::ThreadedGenerateData(
  const OutputImageRegionType & outputRegionForThread,
  ThreadIdType)
{
  const InputImageType * inputPtr = this->GetInput();
  OutputImageType *      outputPtr = this->GetOutput(0);

  // Create a random generator per thread
  IndexValueType indSeed = 0;
  for (unsigned int d = 0; d < TOutputImage::ImageDimension; ++d)
  {
    indSeed += outputRegionForThread.GetIndex(d);
  }
  auto           rand = Statistics::MersenneTwisterRandomVariateGenerator::New();
  const uint32_t seed = Self::Hash(this->GetSeed(), static_cast<uint32_t>(indSeed));
  rand->SetSeed(seed);
  auto randn = Statistics::NormalVariateGenerator::New();
  randn->Initialize(bit_cast<int32_t>(seed));

  // Define the portion of the input to walk for this thread, using
  // the CallCopyOutputRegionToInputRegion method allows for the input
  // and output images to be different dimensions
  InputImageRegionType inputRegionForThread;
  this->CallCopyOutputRegionToInputRegion(inputRegionForThread, outputRegionForThread);

  // Define the iterators
  ImageScanlineConstIterator inputIt(inputPtr, inputRegionForThread);
  ImageScanlineIterator      outputIt(outputPtr, outputRegionForThread);

  TotalProgressReporter progress(this, outputPtr->GetRequestedRegion().GetNumberOfPixels());

  while (!inputIt.IsAtEnd())
  {
    while (!inputIt.IsAtEndOfLine())
    {
      const double in = m_Scale * inputIt.Get();

      // The value of >=50, is the lambda value in a Poisson
      // distribution where a Gaussian distribution is a "good"
      // approximation of the Poisson. This could be considered to be
      // exposed as an advanced parameter in the future.
      if (in < 50)
      {
        const double L = std::exp(-in);
        long         k = 0;
        double       p = 1.0;

        do
        {
          k += 1;
          p *= rand->GetVariate();
        } while (p > L);

        // Clip the output to the actual supported range
        outputIt.Set(Self::ClampCast((k - 1) / m_Scale));
      }
      else
      {
        const double out = in + std::sqrt(in) * randn->GetVariate();
        outputIt.Set(Self::ClampCast(out / m_Scale));
      }
      ++inputIt;
      ++outputIt;
    }
    inputIt.NextLine();
    outputIt.NextLine();
    progress.Completed(outputRegionForThread.GetSize()[0]);
  }
}

template <class TInputImage, class TOutputImage>
void
ShotNoiseImageFilter<TInputImage, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Scale: " << static_cast<typename NumericTraits<double>::PrintType>(m_Scale) << std::endl;
}
} // end namespace itk

#endif
