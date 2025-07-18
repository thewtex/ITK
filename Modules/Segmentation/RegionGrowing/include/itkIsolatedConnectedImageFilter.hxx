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
#ifndef itkIsolatedConnectedImageFilter_hxx
#define itkIsolatedConnectedImageFilter_hxx

#include "itkBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkProgressReporter.h"
#include "itkIterationReporter.h"
#include "itkMath.h"
#include "itkNumericTraits.h"
#include "itkPrintHelper.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::IsolatedConnectedImageFilter()
  : m_Lower(NumericTraits<InputImagePixelType>::NonpositiveMin())
  , m_Upper(NumericTraits<InputImagePixelType>::max())
  , m_ReplaceValue(NumericTraits<OutputImagePixelType>::OneValue())
  , m_IsolatedValue(InputImagePixelType{})
  , m_IsolatedValueTolerance(NumericTraits<InputImagePixelType>::OneValue())
  , m_FindUpperThreshold(true)

{
  m_Seeds1.clear();
  m_Seeds2.clear();
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  using namespace print_helper;

  this->Superclass::PrintSelf(os, indent);

  os << indent << "Seeds1: " << std::endl;
  os << indent << "Seeds2: " << std::endl;

  os << indent << "Lower: " << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_Lower)
     << std::endl;
  os << indent << "Upper: " << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_Upper)
     << std::endl;

  os << indent
     << "ReplaceValue: " << static_cast<typename NumericTraits<OutputImagePixelType>::PrintType>(m_ReplaceValue)
     << std::endl;

  os << indent
     << "IsolatedValue: " << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_IsolatedValue)
     << std::endl;
  os << indent << "IsolatedValueTolerance: "
     << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_IsolatedValueTolerance) << std::endl;

  itkPrintSelfBooleanMacro(FindUpperThreshold);
  itkPrintSelfBooleanMacro(ThresholdingFailed);
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if (this->GetInput())
  {
    const InputImagePointer image = const_cast<TInputImage *>(this->GetInput());
    image->SetRequestedRegionToLargestPossibleRegion();
  }
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::EnlargeOutputRequestedRegion(DataObject * output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::AddSeed1(const IndexType & seed)
{
  this->m_Seeds1.push_back(seed);
  this->Modified();
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::SetSeed1(const IndexType & seed)
{
  this->ClearSeeds1();
  this->AddSeed1(seed);
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::ClearSeeds1()
{
  if (!this->m_Seeds1.empty())
  {
    this->m_Seeds1.clear();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::AddSeed2(const IndexType & seed)
{
  this->m_Seeds2.push_back(seed);
  this->Modified();
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::SetSeed2(const IndexType & seed)
{
  this->ClearSeeds2();
  this->AddSeed2(seed);
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::ClearSeeds2()
{
  if (!this->m_Seeds2.empty())
  {
    this->m_Seeds2.clear();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutputImage>
auto
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::GetSeeds1() const -> const SeedsContainerType &
{
  itkDebugMacro("returning Seeds1");
  return this->m_Seeds1;
}

template <typename TInputImage, typename TOutputImage>
auto
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::GetSeeds2() const -> const SeedsContainerType &
{
  itkDebugMacro("returning Seeds2");
  return this->m_Seeds2;
}

template <typename TInputImage, typename TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  const InputImageConstPointer inputImage = this->GetInput();
  const OutputImagePointer     outputImage = this->GetOutput();

  using AccumulateType = typename NumericTraits<InputImagePixelType>::AccumulateType;

  if (m_Seeds1.empty())
  {
    itkExceptionMacro("Seeds1 container is empty");
  }

  if (m_Seeds2.empty())
  {
    itkExceptionMacro("Seeds2 container is empty");
  }

  // Zero the output
  const OutputImageRegionType region = outputImage->GetRequestedRegion();
  outputImage->SetBufferedRegion(region);
  outputImage->AllocateInitialized();

  using FunctionType = BinaryThresholdImageFunction<InputImageType>;
  using IteratorType = FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType>;

  auto function = FunctionType::New();
  function->SetInputImage(inputImage);

  float             progressWeight = 0.0f;
  float             cumulatedProgress = 0.0f;
  IteratorType      it(outputImage, function, m_Seeds1);
  IterationReporter iterate(this, 0, 1);

  // If the upper threshold has not been set, find it.
  if (m_FindUpperThreshold)
  {
    auto           lower = static_cast<AccumulateType>(m_Lower);
    auto           upper = static_cast<AccumulateType>(m_Upper);
    AccumulateType guess = upper;

    // do a binary search to find an upper threshold that separates the
    // two sets of seeds.
    const auto maximumIterationsInBinarySearch = static_cast<unsigned int>(
      std::log((static_cast<float>(upper) - static_cast<float>(lower)) / static_cast<float>(m_IsolatedValueTolerance)) /
      std::log(2.0));

    progressWeight = 1.0f / static_cast<float>(maximumIterationsInBinarySearch + 2);
    cumulatedProgress = 0.0f;

    while (lower + m_IsolatedValueTolerance < guess)
    {
      ProgressReporter progress(this, 0, region.GetNumberOfPixels(), 100, cumulatedProgress, progressWeight);
      cumulatedProgress += progressWeight;
      outputImage->FillBuffer(OutputImagePixelType{});
      function->ThresholdBetween(m_Lower, static_cast<InputImagePixelType>(guess));
      it.GoToBegin();
      while (!it.IsAtEnd())
      {
        it.Set(m_ReplaceValue);
        if (it.GetIndex() == m_Seeds2.front())
        {
          break;
        }
        ++it;
        progress.CompletedPixel(); // potential exception thrown here
      }
      // If any of second seeds are included, decrease the upper bound.
      // Find the sum of the intensities in m_Seeds2.  If the second
      // seeds are not included, the sum should be zero.  Otherwise,
      // it will be other than zero.
      InputRealType seedIntensitySum{};
      auto          si = m_Seeds2.begin();
      const auto    li = m_Seeds2.end();
      while (si != li)
      {
        const auto value = static_cast<InputRealType>(outputImage->GetPixel(*si));
        seedIntensitySum += value;
        ++si;
      }

      if (Math::NotExactlyEquals(seedIntensitySum, InputRealType{}))
      {
        upper = guess;
      }
      // Otherwise, increase the lower bound.
      else
      {
        lower = guess;
      }
      guess = (upper + lower) / 2;
      iterate.CompletedStep();
    }

    m_IsolatedValue = static_cast<InputImagePixelType>(lower); // the lower
                                                               // bound on
                                                               // the upper
                                                               // threshold
                                                               // guess
  }
  else
  { // If the lower threshold has not been set, find it.
    auto           lower = static_cast<AccumulateType>(m_Lower);
    auto           upper = static_cast<AccumulateType>(m_Upper);
    AccumulateType guess = lower;

    // do a binary search to find a lower threshold that separates the
    // two sets of seeds.
    const auto maximumIterationsInBinarySearch = static_cast<unsigned int>(
      std::log((static_cast<float>(upper) - static_cast<float>(lower)) / static_cast<float>(m_IsolatedValueTolerance)) /
      std::log(2.0));

    progressWeight = 1.0f / static_cast<float>(maximumIterationsInBinarySearch + 2);
    cumulatedProgress = 0.0f;

    while (guess < upper - m_IsolatedValueTolerance)
    {
      ProgressReporter progress(this, 0, region.GetNumberOfPixels(), 100, cumulatedProgress, progressWeight);
      cumulatedProgress += progressWeight;
      outputImage->FillBuffer(OutputImagePixelType{});
      function->ThresholdBetween(static_cast<InputImagePixelType>(guess), m_Upper);
      it.GoToBegin();
      while (!it.IsAtEnd())
      {
        it.Set(m_ReplaceValue);
        if (it.GetIndex() == m_Seeds2.front())
        {
          break;
        }
        ++it;
        progress.CompletedPixel(); // potential exception thrown here
      }
      // If any of second seeds are included, increase the lower bound.
      // Find the sum of the intensities in m_Seeds2.  If the second
      // seeds are not included, the sum should be zero.  Otherwise,
      // it will be other than zero.
      InputRealType seedIntensitySum{};
      auto          si = m_Seeds2.begin();
      const auto    li = m_Seeds2.end();
      while (si != li)
      {
        const auto value = static_cast<InputRealType>(outputImage->GetPixel(*si));
        seedIntensitySum += value;
        ++si;
      }

      if (Math::NotExactlyEquals(seedIntensitySum, InputRealType{}))
      {
        lower = guess;
      }
      // Otherwise, decrease the upper bound.
      else
      {
        upper = guess;
      }
      guess = (upper + lower) / 2;
      iterate.CompletedStep();
    }

    m_IsolatedValue = static_cast<InputImagePixelType>(upper); // the upper
                                                               // bound on
                                                               // the lower
                                                               // threshold
                                                               // guess
  }

  // now rerun the algorithm with the thresholds that separate the seeds.
  ProgressReporter progress(this, 0, region.GetNumberOfPixels(), 100, cumulatedProgress, progressWeight);

  outputImage->FillBuffer(OutputImagePixelType{});
  if (m_FindUpperThreshold)
  {
    function->ThresholdBetween(m_Lower, m_IsolatedValue);
  }
  else
  {
    function->ThresholdBetween(m_IsolatedValue, m_Upper);
  }
  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    it.Set(m_ReplaceValue);
    ++it;
    progress.CompletedPixel(); // potential exception thrown here
  }

  // If any of the second seeds are included or some of the first
  // seeds are not included, the algorithm could not find any threshold
  // that would separate the two sets of seeds.  Set an error flag in
  // this case.

  // Find the sum of the intensities in m_Seeds2.  If the second
  // seeds are not included, the sum should be zero.  Otherwise,
  // it will be other than zero.
  InputRealType seed1IntensitySum{};
  InputRealType seed2IntensitySum{};
  auto          si1 = m_Seeds1.begin();
  const auto    li1 = m_Seeds1.end();
  while (si1 != li1)
  {
    const auto value = static_cast<InputRealType>(outputImage->GetPixel(*si1));
    seed1IntensitySum += value;
    ++si1;
  }
  auto       si2 = m_Seeds2.begin();
  const auto li2 = m_Seeds2.end();
  while (si2 != li2)
  {
    const auto value = static_cast<InputRealType>(outputImage->GetPixel(*si2));
    seed2IntensitySum += value;
    ++si2;
  }
  if (Math::NotAlmostEquals(seed1IntensitySum, m_ReplaceValue * m_Seeds1.size()) ||
      Math::NotExactlyEquals(seed2IntensitySum, InputRealType{}))
  {
    m_ThresholdingFailed = true;
  }
  iterate.CompletedStep();
}
} // end namespace itk

#endif
