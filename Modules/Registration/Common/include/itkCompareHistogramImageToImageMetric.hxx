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
#ifndef itkCompareHistogramImageToImageMetric_hxx
#define itkCompareHistogramImageToImageMetric_hxx

#include "itkHistogram.h"

namespace itk
{
template <typename TFixedImage, typename TMovingImage>
CompareHistogramImageToImageMetric<TFixedImage, TMovingImage>::CompareHistogramImageToImageMetric()
  : m_TrainingFixedImage(nullptr)
  , m_TrainingMovingImage(nullptr)
  , m_TrainingTransform(nullptr)
  , m_TrainingInterpolator(nullptr)
  , m_TrainingHistogram(nullptr) // either provided by the user or created
{}

template <typename TFixedImage, typename TMovingImage>
void
CompareHistogramImageToImageMetric<TFixedImage, TMovingImage>::Initialize()
{
  Superclass::Initialize();

  if (!m_TrainingHistogram)
  {
    FormTrainingHistogram();
  }
}

template <typename TFixedImage, typename TMovingImage>
void
CompareHistogramImageToImageMetric<TFixedImage, TMovingImage>::FormTrainingHistogram()
{
  // Check to make sure everything is set
  if (!m_TrainingTransform)
  {
    itkExceptionMacro("Training Transform is not present");
  }

  if (!m_TrainingInterpolator)
  {
    itkExceptionMacro("Training Interpolator is not present");
  }

  if (!m_TrainingMovingImage)
  {
    itkExceptionMacro("Training MovingImage is not present");
  }

  // If the image is provided by a source, update the source.
  m_TrainingMovingImage->UpdateSource();

  if (!m_TrainingFixedImage)
  {
    itkExceptionMacro("Training FixedImage is not present");
  }

  // If the image is provided by a source, update the source.
  m_TrainingFixedImage->UpdateSource();

  if (m_TrainingFixedImageRegion.GetNumberOfPixels() == 0)
  {
    itkExceptionMacro("TrainingFixedImageRegion is empty");
  }

  // Make sure the FixedImageRegion is within the FixedImage buffered region
  if (!m_TrainingFixedImageRegion.Crop(m_TrainingFixedImage->GetBufferedRegion()))
  {
    itkExceptionMacro("TrainingFixedImageRegion does not overlap the training fixed image buffered region");
  }

  this->m_TrainingInterpolator->SetInputImage(GetTrainingMovingImage());

  // Create the exact histogram structure as the one to be used
  // to evaluate the metric. This code is mostly copied
  // from itkHistogramImageToImageMetric
  this->m_TrainingHistogram = HistogramType::New();
  this->m_TrainingHistogram->SetMeasurementVectorSize(2);
  this->m_TrainingHistogram->Initialize(
    this->Superclass::m_HistogramSize, this->Superclass::m_LowerBound, this->Superclass::m_UpperBound);
  using TrainingFixedIteratorType = itk::ImageRegionConstIteratorWithIndex<FixedImageType>;
  typename FixedImageType::IndexType index;
  typename HistogramType::IndexType  hIndex;

  TrainingFixedIteratorType ti(this->m_TrainingFixedImage, this->m_TrainingFixedImageRegion);

  int NumberOfPixelsCounted = 0;

  ti.GoToBegin();
  while (!ti.IsAtEnd())
  {
    index = ti.GetIndex();
    if (this->m_TrainingFixedImageRegion.IsInside(index) &&
        (!this->Superclass::GetUsePaddingValue() ||
         (this->Superclass::GetUsePaddingValue() && ti.Get() > this->Superclass::GetPaddingValue())))
    {
      typename Superclass::InputPointType inputPoint;
      this->m_TrainingFixedImage->TransformIndexToPhysicalPoint(index, inputPoint);

      const typename Superclass::OutputPointType transformedPoint =
        this->m_TrainingTransform->TransformPoint(inputPoint);

      if (this->m_TrainingInterpolator->IsInsideBuffer(transformedPoint))
      {
        const RealType TrainingMovingValue = this->m_TrainingInterpolator->Evaluate(transformedPoint);
        const RealType TrainingFixedValue = ti.Get();
        ++NumberOfPixelsCounted;

        typename HistogramType::MeasurementVectorType sample;
        sample.SetSize(2);
        sample[0] = TrainingFixedValue;
        sample[1] = TrainingMovingValue;

        this->m_TrainingHistogram->GetIndex(sample, hIndex);
        this->m_TrainingHistogram->IncreaseFrequencyOfIndex(hIndex, 1);
      }
    }

    ++ti;
  }

  if (NumberOfPixelsCounted == 0)
  {
    itkExceptionMacro("All the points mapped to outside of the Training moving age");
  }
}

template <typename TFixedImage, typename TMovingImage>
void
CompareHistogramImageToImageMetric<TFixedImage, TMovingImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  itkPrintSelfObjectMacro(TrainingFixedImage);
  itkPrintSelfObjectMacro(TrainingMovingImage);
  itkPrintSelfObjectMacro(TrainingTransform);
  itkPrintSelfObjectMacro(TrainingInterpolator);

  os << indent << "TrainingFixedImageRegion: " << m_TrainingFixedImageRegion << std::endl;

  itkPrintSelfObjectMacro(TrainingHistogram);
}

} // end namespace itk

#endif // itkCompareHistogramImageToImageMetric_hxx
