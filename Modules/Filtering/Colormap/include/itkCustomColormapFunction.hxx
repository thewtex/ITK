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
#ifndef itkCustomColormapFunction_hxx
#define itkCustomColormapFunction_hxx

namespace itk::Function
{
template <typename TScalar, typename TRGBPixel>
auto
CustomColormapFunction<TScalar, TRGBPixel>::operator()(const TScalar & v) const -> RGBPixelType
{
  // Map the input scalar between [0, 1].
  const RealType value = this->RescaleInputValue(v);

  // Setup some arrays and apply color mapping
  enum ColorNames
  {
    RED = 0,
    GREEN = 1,
    BLUE = 2
  };
  RealType                  RGBValue[3] = { 0.0 };
  const ChannelType * const ColorChannel[3] = { &m_RedChannel, &m_GreenChannel, &m_BlueChannel };

  for (size_t color = RED; color <= BLUE; ++color) // Go through all the colors
  {
    const size_t size = ColorChannel[color]->size();
    auto         index = Math::Ceil<size_t, RealType>(value * static_cast<RealType>(size - 1));

    if (size == 1 || index < 1)
    {
      RGBValue[color] = (*ColorChannel[color])[0];
    }
    else if (size > 1)
    {
      const RealType p1 = (*ColorChannel[color])[index];
      const RealType m1 = (*ColorChannel[color])[index - 1u];
      const RealType d = p1 - m1;
      RGBValue[color] = d * (size - 1) * (value - (index - 1) / static_cast<RealType>(size - 1)) + m1;
    }
  }

  // Set the RGB components after rescaling the values.
  RGBPixelType pixel;
  NumericTraits<TRGBPixel>::SetLength(pixel, 3);

  for (size_t color{ RED }; color <= size_t{ BLUE }; ++color)
  {
    pixel[color] = this->RescaleRGBComponentValue(RGBValue[color]);
  }

  return pixel;
}
} // namespace itk::Function

#endif
