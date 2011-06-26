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
#ifndef __itkMetaObjectConverterFactory_h
#define __itkMetaObjectConverterFactory_h
#include <map>
#include "itkSpatialObject.h"
class MetaObject;

namespace itk
{
class MetaObjectConverterFactory
{
public:
  typedef SpatialObject<2> SpatialObject2D;
  typedef SpatialObject<3> SpatialObject3D;
  typedef MetaObject * (*MetaObjectConversionFunction)(void *);
  typedef std::map<const std::string,MetaObjectConversionFunction> MapType;
  static MetaObject *Convert(SpatialObject2D *);
  static MetaObject *Convert(SpatialObject3D *);
  static void RegisterConverter(const char *className,MetaObjectConversionFunction converter);
private:
  static MapType m_ConverterMap;
};

}

#endif // __itkMetaObjectConverterFactory_h
