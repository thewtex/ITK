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
#ifndef __itkHSVColormapFunction_hpp
#define __itkHSVColormapFunction_hpp

#include "itkHSVColormapFunction.h"

namespace itk
{
namespace Function
{
template< class TScalar, class TRGBPixel >
typename HSVColormapFunction< TScalar, TRGBPixel >::RGBPixelType
HSVColormapFunction< TScalar, TRGBPixel >
::operator()(const TScalar & v) const
{
  // Map the input scalar between [0, 1].
  RealType value = this->RescaleInputValue(v);

  // Apply the color mapping.
  // Apply the color mapping.
  RealType red = vnl_math_abs( 5.0 * ( value - 0.5 ) ) - 5.0 / 6.0;

  red = vnl_math_min(red, 1.0);
  red = vnl_math_max(0.0, red);

  RealType green = -vnl_math_abs( 5.0 * ( value - 11.0 / 30.0 ) ) + 11.0 / 6.0;
  green = vnl_math_min(green, 1.0);
  green = vnl_math_max(0.0, green);

  RealType blue = -vnl_math_abs( 5.0 * ( value - 19.0 / 30.0 ) ) + 11.0 / 6.0;
  blue = vnl_math_min(blue, 1.0);
  blue = vnl_math_max(0.0, blue);

  // Set the rgb components after rescaling the values.
  RGBPixelType pixel;

  pixel[0] = this->RescaleRGBComponentValue(red);
  pixel[1] = this->RescaleRGBComponentValue(green);
  pixel[2] = this->RescaleRGBComponentValue(blue);

  return pixel;
}
} // end namespace Function
} // end namespace itk

#endif
