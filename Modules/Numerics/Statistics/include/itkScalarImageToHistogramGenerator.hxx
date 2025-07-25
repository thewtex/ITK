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
#ifndef itkScalarImageToHistogramGenerator_hxx
#define itkScalarImageToHistogramGenerator_hxx

namespace itk::Statistics
{
template <typename TImage>
ScalarImageToHistogramGenerator<TImage>::ScalarImageToHistogramGenerator()
  : m_ImageToListSampleAdaptor(AdaptorType::New())
  , m_HistogramGenerator(GeneratorType::New())
{
  m_HistogramGenerator->SetInput(m_ImageToListSampleAdaptor);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetInput(const ImageType * image)
{
  m_ImageToListSampleAdaptor->SetImage(image);
}

template <typename TImage>
auto
ScalarImageToHistogramGenerator<TImage>::GetOutput() const -> const HistogramType *
{
  return m_HistogramGenerator->GetOutput();
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::Compute()
{
  m_HistogramGenerator->Update();
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetNumberOfBins(unsigned int numberOfBins)
{
  typename HistogramType::SizeType size;
  size.SetSize(1);
  size.Fill(numberOfBins);
  m_HistogramGenerator->SetHistogramSize(size);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetHistogramMin(RealPixelType minimumValue)
{
  using MeasurementVectorType = typename GeneratorType::HistogramMeasurementVectorType;
  MeasurementVectorType minVector;
  NumericTraits<MeasurementVectorType>::SetLength(minVector, 1);
  minVector[0] = minimumValue;
  m_HistogramGenerator->SetHistogramBinMinimum(minVector);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetHistogramMax(RealPixelType maximumValue)
{
  using MeasurementVectorType = typename GeneratorType::HistogramMeasurementVectorType;
  MeasurementVectorType maxVector;
  NumericTraits<MeasurementVectorType>::SetLength(maxVector, 1);
  maxVector[0] = maximumValue;
  m_HistogramGenerator->SetHistogramBinMaximum(maxVector);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetAutoHistogramMinimumMaximum(bool autoOnOff)
{
  m_HistogramGenerator->SetAutoMinimumMaximum(autoOnOff);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::SetMarginalScale(double marginalScale)
{
  m_HistogramGenerator->SetMarginalScale(marginalScale);
}

template <typename TImage>
void
ScalarImageToHistogramGenerator<TImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  itkPrintSelfObjectMacro(ImageToListSampleAdaptor);
  itkPrintSelfObjectMacro(HistogramGenerator);
}
} // namespace itk::Statistics

#endif
