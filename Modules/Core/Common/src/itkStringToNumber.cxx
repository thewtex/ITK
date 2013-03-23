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
#include "itkStringToNumber.h"
#include "itkNumericTraits.h"

#include <sstream>

namespace itk
{

template<>
double StringToNumber<double>::operator() (std::string val)
{
  int flags = 0;
  double emptyStringValue = 0.0;
  double junkStringValue = NumericTraits< double >::quiet_NaN();
  const char * infinitySymbol = "Inf";
  const char * nanSymbol = "Nan";
  double_conversion::StringToDoubleConverter converter(
    flags,
    emptyStringValue,
    junkStringValue,
    infinitySymbol,
    nanSymbol);

  int processedCharacterCount;

  return converter.StringToDouble(
    val.c_str(), val.size(),
    &processedCharacterCount);
}

template<>
float StringToNumber<float>::operator() (std::string val)
{
  int flags = 0;
  double emptyStringValue = 0.0;
  double junkStringValue = NumericTraits< double >::quiet_NaN();
  const char * infinitySymbol = "Inf";
  const char * nanSymbol = "Nan";
  double_conversion::StringToDoubleConverter converter(
    flags,
    emptyStringValue,
    junkStringValue,
    infinitySymbol,
    nanSymbol);

  int processedCharacterCount;

  return converter.StringToFloat(
    val.c_str(), val.size(),
    &processedCharacterCount);
}
}
