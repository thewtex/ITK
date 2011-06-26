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
#include "itkMetaObjectConverterFactory.h"
#include "itkMacro.h"
#include "metaObject.h"

namespace itk
{
MetaObjectConverterFactory::MapType MetaObjectConverterFactory::m_ConverterMap;

MetaObject *
MetaObjectConverterFactory
::Convert(SpatialObject2D *objectToConvert)
{
  if(objectToConvert == 0)
    {
    itkGenericExceptionMacro(<< "Null object as argument ");
    }
  const std::string metaTypeName(objectToConvert->GetTypeName());
  MapType::iterator it = m_ConverterMap.find(metaTypeName);
  if(it == m_ConverterMap.end())
    {
    itkGenericExceptionMacro(<< "Can't convert Unknown MetaObject type " <<
                          metaTypeName);
    }
  return (it->second)(objectToConvert);
}
MetaObject *
MetaObjectConverterFactory
::Convert(SpatialObject3D *objectToConvert)
{
  if(objectToConvert == 0)
    {
    itkGenericExceptionMacro(<< "Null object as argument ");
    }
  const std::string metaTypeName(objectToConvert->GetTypeName());
  MapType::iterator it = m_ConverterMap.find(metaTypeName);
  if(it == m_ConverterMap.end())
    {
    itkGenericExceptionMacro(<< "Can't convert Unknown MetaObject type " <<
                          metaTypeName);
    }
  return (it->second)(objectToConvert);
}

void
MetaObjectConverterFactory
::RegisterConverter(const char *className,MetaObjectConversionFunction converter)
{
  const std::string metaTypeName(className);
  MapType::iterator it = m_ConverterMap.find(metaTypeName);
  if(it != m_ConverterMap.end())
    {
    it->second = converter;
    }
  else
    {
    m_ConverterMap[metaTypeName] = converter;
    }
}
}
