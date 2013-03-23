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
#ifndef __itkStringToNumber_h
#define __itkStringToNumber_h

#include "double-conversion.h"
#include "itkMacro.h"
#include "itkNumericTraits.h"

#include <string>

namespace itk
{
/** \class StringToNumber
 * \brief Convert strings to numbers
 *
 * This class uses the double-conversion library to convert ASCII
 * strings to numbers.
 *
 * Typical use:
 *  #include "itkStringToNumber.h"
 *  StringToNumber<float> convert;
 *  std::string a = "1.333";
 *  float b;
 *  b = convert(a);
 *
 * \ingroup ITKCommon
 */
template< typename TValueType>
class StringToNumber
{
public:
  StringToNumber() {}
  typedef TValueType ValueType;
  ValueType operator() (std::string val);

private:
  StringToNumber & operator=(const StringToNumber &); // not defined
};

// declaration of specialization
template<> ITKCommon_EXPORT double StringToNumber<double>::operator()(std::string val);
template<> ITKCommon_EXPORT float StringToNumber<float>::operator()(std::string val);

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStringToNumber.hxx"
#endif
#endif
