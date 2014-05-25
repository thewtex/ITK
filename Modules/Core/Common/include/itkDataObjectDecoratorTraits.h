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
#ifndef __itkDataObjectDecoratorTraits_h
#define __itkDataObjectDecoratorTraits_h

#include "itkObject.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itkDataObjectDecorator.h"

namespace itk
{

template<typename T> struct DataObjectDecoratorTraits;


/** \cond SPECIALIZATION_IMPLEMENTATION */
namespace Detail
{

// note: availability of std::(tr1::)is_base_of and std::conditional
// could lead to a cleaner implementation
template<typename TObject, typename T, typename U=void>
struct DataObjectDecoratorTraitsHelper
{

  typedef SimpleDataObjectDecorator<TObject> Type;
};

template<typename TObject>
struct DataObjectDecoratorTraitsHelper<TObject, Object, void >
{
  typedef DataObjectDecorator<TObject> Type;
};

// the VoidType is only used to enable SFINAE, by making the Superclass
// type dependent.
template<typename TObject,typename T>
struct DataObjectDecoratorTraitsHelper<TObject, T, typename DataObjectDecoratorTraits<typename T::Superclass>::VoidType >
{
  typedef typename DataObjectDecoratorTraitsHelper<TObject, typename T::Superclass>::Type Type;
};

}
/**\endcond*/

/** \brief A class to determine which DataObjectDecorator class to use
 *
 * If type T is derived from an itk::Object then
 * itk::DataObjectDecorator<T> will be Type, otherwise
 * itk::SimpleDataObjectDecorator<T> will be the Type.
*/
template<typename T>
struct DataObjectDecoratorTraits
{
  typedef typename Detail::DataObjectDecoratorTraitsHelper<T,T>::Type Type;

private:
  template<typename, typename, typename> friend struct DataObjectDecoratorTraitsHelper;
  typedef void VoidType;
};


}

#endif //__itkDataObjectDecoratorTraits_h
