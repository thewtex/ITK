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
#include "itkTransformFileWriter.hxx"
namespace itk
{

namespace
{
template<>
void AddToTransformList<double,double>(TransformBaseTemplate<double>::ConstPointer &transform,
                                        std::list< TransformBaseTemplate<double>::ConstPointer > & transformList)
{
  transformList.push_back( TransformBaseTemplate<double>::ConstPointer(transform) );
}

template<>
void AddToTransformList<float,float>(TransformBaseTemplate<float>::ConstPointer &transform,
                                      std::list< TransformBaseTemplate<float>::ConstPointer > & transformList)
{
  transformList.push_back( TransformBaseTemplate<float>::ConstPointer(transform) );
}
}

template<>
void TransformFileWriterTemplate<double>
::PushBackTransformList(const Object *transObj)
{
  TransformBaseTemplate<double>::ConstPointer dblptr = dynamic_cast<const TransformBaseTemplate<double> *>( transObj );
  if( dblptr.IsNotNull() )
    {
    AddToTransformList<double, double>( dblptr, m_TransformList );
    }
  else
    {
    TransformBaseTemplate<float>::ConstPointer fltptr = dynamic_cast<const TransformBaseTemplate<float> *>( transObj );
    if( fltptr.IsNotNull() )
      {
      AddToTransformList<double, float>( fltptr, m_TransformList );
      }
    else
      {
      itkExceptionMacro("The input of writer should be whether a double precision"
                        "or a single precision transform type.");
      }
    }
}

template<>
void TransformFileWriterTemplate<float>
::PushBackTransformList(const Object *transObj)
{
  TransformBaseTemplate<double>::ConstPointer dblptr = dynamic_cast<const TransformBaseTemplate<double> *>( transObj );
  if( dblptr.IsNotNull() )
    {
    AddToTransformList<float, double>( dblptr, m_TransformList );
    }
  else
    {
    TransformBaseTemplate<float>::ConstPointer fltptr = dynamic_cast<const TransformBaseTemplate<float> *>( transObj );
    if( fltptr.IsNotNull() )
      {
      AddToTransformList<float, float>( fltptr, m_TransformList );
      }
    else
      {
      itkExceptionMacro("The input of writer should be whether a double precision"
                        "or a single precision transform type.");
      }
    }
}

}
