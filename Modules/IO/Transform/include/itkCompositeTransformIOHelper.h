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
#ifndef __itkCompositeTransformIOHelper_h
#define __itkCompositeTransformIOHelper_h

#include "itkTransformIOBase.h"

namespace itk
{

class CompositeTransformIOHelper
{
public:
  typedef TransformIOBase::TransformType          TransformType;
  typedef TransformIOBase::TransformPointer       TransformPointer;
  typedef TransformIOBase::TransformListType      TransformListType;
  typedef TransformIOBase::ConstTransformPointer  ConstTransformPointer;
  typedef TransformIOBase::ConstTransformListType ConstTransformListType;

  /** from a composite transform recover a TransformIOBase::ConstTransformList
   */
  ConstTransformListType &GetTransformList(const TransformType *transform);
  /** set a compositeTransform's transform list from a
   **  TransformIOABase::TransormList
  */
  void SetTransformList(TransformType *transform,TransformListType &transformList);
private:
  ConstTransformListType m_TransformList;

  template <typename TScalar, unsigned TDim>
  int BuildTransformList(const TransformType *transform);

  template <typename TScalar, unsigned TDim>
  int InternalSetTransformList(TransformType *transform,TransformListType &transformList);

};


}
#endif //  __itkCompositeTransformIOHelper_h
