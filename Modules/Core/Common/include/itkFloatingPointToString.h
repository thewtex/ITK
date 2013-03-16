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
#ifndef __itkFloatingPointToString_h
#define __itkFloatingPointToString_h

#include "double-conversion.h"
#include "itkMacro.h"
#include <string>

namespace itk
{
template< typename TValueType>
class FloatingPointToString
{
public:
  FloatingPointToString() :
    m_DoubleToStringConverter(double_conversion::DoubleToStringConverter::EcmaScriptConverter())
    {
    }
  std::string operator() (TValueType val);

private:
  FloatingPointToString & operator=(const FloatingPointToString &); // not defined
  const double_conversion::DoubleToStringConverter &m_DoubleToStringConverter;
};

#ifndef _MSC_VER
// declaration of specialization
template<> std::string FloatingPointToString<double>::operator() (double val);
template<> std::string FloatingPointToString<float>::operator() (float val);
#endif

// export specification for explicit instantiation
template ITKCommon_EXPORT std::string FloatingPointToString<double>::operator()(double val);
template ITKCommon_EXPORT std::string FloatingPointToString<float>::operator()(float val);

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFloatingPointToString.hxx"
#endif
#endif
