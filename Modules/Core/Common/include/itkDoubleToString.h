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
#ifndef __itkDoubleToString_h
#define __itkDoubleToString_h

#include "double-conversion.h"
#include "itkMacro.h"

namespace itk
{
template< typename TValueType>
class DoubleToString
{
public:
  DoubleToString() :
    m_DoubleToStringConverter(double_conversion::DoubleToStringConverter::EcmaScriptConverter())
    {
    }
  std::string operator()(TValueType val)
    {
      char buf[256];
      double_conversion::StringBuilder builder(buf,sizeof(buf));
      builder.Reset();
      if (typeid(TValueType) == typeid(float))
        {
        if(!m_DoubleToStringConverter.ToShortestSingle(val,&builder))
          {
          itkGenericExceptionMacro(<< "Conversion failed for " << val);
          }
        }
      else
        {
        if(!m_DoubleToStringConverter.ToShortest(val,&builder))
          {
          itkGenericExceptionMacro(<< "Conversion failed for " << val);
          }
        }
      return std::string(builder.Finalize());
    }
private:
  DoubleToString & operator=(const DoubleToString &); // not defined
  const double_conversion::DoubleToStringConverter &m_DoubleToStringConverter;
};

} // namespace itk
#endif
