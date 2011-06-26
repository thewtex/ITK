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
#include "itkSpatialObjectConverterFactory.h"
#include "itkMacro.h"
#include "itkSpatialObject.h"
#include "metaObject.h"

namespace itk
{

template<unsigned int NDimensions> itk::SimpleFastMutexLock                          itk::SpatialObjectConverterFactory<NDimensions>::m_CreationLock;
template<unsigned int NDimensions> itk::SpatialObjectConverterFactory<NDimensions> * itk::SpatialObjectConverterFactory<NDimensions>::m_Instance=NULL;

template<unsigned int NDimension>
typename SpatialObjectConverterFactory<NDimension>::SpatialObjectPointer
SpatialObjectConverterFactory<NDimension>
::Convert(MetaObject *objectToConvert)
{
  if(objectToConvert == 0)
    {
    itkGenericExceptionMacro(<< "Null object as argument ");
    }
  const std::string metaTypeName(objectToConvert->ObjectTypeName());
  typename MapType::iterator it = this->m_ConverterMap.find(metaTypeName);
  if(it == this->m_ConverterMap.end())
    {
    itkGenericExceptionMacro(<< "Can't convert Unknown SpatialObject type " <<
                          metaTypeName);
    }
  typename SpatialObjectConverterFactory<NDimension>::SpatialObjectPointer temp= (it->second)(objectToConvert) ;
  return temp;
}


template<unsigned int NDimension>
void
SpatialObjectConverterFactory<NDimension>
::RegisterConverter(const char *className,SpatialObjectConverterFactory<NDimension>::SpatialObjectConversionFunction converter)
{
  const std::string metaTypeName(className);
  typename MapType::iterator it = m_ConverterMap.find(metaTypeName);
  if(it != m_ConverterMap.end())
    {
    it->second = converter;
    }
  else
    {
    m_ConverterMap[metaTypeName] = converter;
    }
}

template <unsigned int NDimensions>
SpatialObjectConverterFactory<NDimensions> *
SpatialObjectConverterFactory<NDimensions>
::GetInstance()
{
  if( ! SpatialObjectConverterFactory<NDimensions>::m_Instance )
    {
    SpatialObjectConverterFactory<NDimensions>::m_CreationLock.Lock();
    //Need to make sure that during gaining access
    //to the lock that some other thread did not
    //initialize the singleton.
    if( ! SpatialObjectConverterFactory<NDimensions>::m_Instance )
      {
      SpatialObjectConverterFactory<NDimensions>::m_Instance= new SpatialObjectConverterFactory<NDimensions>;
      if ( ! SpatialObjectConverterFactory<NDimensions>::m_Instance )
        {
        std::ostringstream message;
        message << "itk::ERROR: " << "SpatialObjectConverterFactory"
          << " Valid SpatialObjectConverterFactory instance not created";
        ::itk::ExceptionObject e_(__FILE__, __LINE__, message.str().c_str(), ITK_LOCATION);
        throw e_; /* Explicit naming to work around Intel compiler bug.  */
        }
      }
    SpatialObjectConverterFactory<NDimensions>::m_CreationLock.Unlock();
    }
  return SpatialObjectConverterFactory<NDimensions>::m_Instance;
}


template class SpatialObjectConverterFactory<2>;
template class SpatialObjectConverterFactory<3>;


}
