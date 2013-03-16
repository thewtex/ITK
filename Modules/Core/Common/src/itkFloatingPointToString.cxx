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
#include "itkFloatingPointToString.h"
#include "itkNumericTraits.h"

#include <sstream>

namespace itk
{

template<>
std::string FloatingPointToString<double>::operator() (double val)
{
  char buf[256];
  double_conversion::StringBuilder builder(buf,sizeof(buf));
  builder.Reset();
  if(!m_DoubleToStringConverter.ToShortest(val,&builder))
    {
    itkGenericExceptionMacro(<< "Conversion failed for " << val);
    }
  return std::string(builder.Finalize());
}

template<>
std::string FloatingPointToString<float>::operator() (float val)
{
  char buf[256];
  double_conversion::StringBuilder builder(buf,sizeof(buf));
  builder.Reset();
  if(!m_DoubleToStringConverter.ToShortestSingle(val,&builder))
    {
    itkGenericExceptionMacro(<< "Conversion failed for " << val);
    }
  return std::string(builder.Finalize());
}

}
