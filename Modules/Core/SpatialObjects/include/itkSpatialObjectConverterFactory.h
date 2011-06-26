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
#ifndef __itkSpatialObjectConverterFactory_h
#define __itkSpatialObjectConverterFactory_h
#include <map>
#include "itkSpatialObject.h"
#include "metaObject.h"

namespace itk
{

template<unsigned int NDimensions>
class SpatialObjectConverterFactory
{
public:
  typedef          SpatialObject<NDimensions> SpatialObjectType;
  typedef typename SpatialObjectType::Pointer SpatialObjectPointer;

  typedef SpatialObjectPointer (*SpatialObjectConversionFunction)(MetaObject *);

  SpatialObjectPointer Convert(MetaObject *);
  void RegisterConverter(const char *className,SpatialObjectConversionFunction converter);
  typedef std::map<const std::string,SpatialObjectConversionFunction> MapType;

  static SpatialObjectConverterFactory<NDimensions> * GetInstance();
private:
  SpatialObjectConverterFactory(): m_ConverterMap() {}

  MapType m_ConverterMap;
  static SimpleFastMutexLock                          m_CreationLock;
  static SpatialObjectConverterFactory<NDimensions> * m_Instance;
};


}

#endif // __itkSpatialObjectConverterFactory_h
