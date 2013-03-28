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
#include "itkMatlabTransformIOFactoryTemplate.h"

template class itk::MatlabTransformIOFactoryTemplate<double>;
template class itk::MatlabTransformIOFactoryTemplate<float>;

namespace itk
{
    // Undocumented API used to register during static initialization.
    // DO NOT CALL DIRECTLY.
  static bool MatlabTransformIOFactoryTemplateFloatHasBeenRegistered = false;

  void MatlabTransformIOFactoryTemplateFloatRegister__Private(void)
  {
  if( ! MatlabTransformIOFactoryTemplateFloatHasBeenRegistered )
    {
    MatlabTransformIOFactoryTemplateFloatHasBeenRegistered = true;
    MatlabTransformIOFactoryTemplate<float>::RegisterOneFactory();
    }
  }

  static bool MatlabTransformIOFactoryTemplateDoubleHasBeenRegistered = false;

  void MatlabTransformIOFactoryTemplateDoubleRegister__Private(void)
  {
  if( ! MatlabTransformIOFactoryTemplateDoubleHasBeenRegistered )
    {
    MatlabTransformIOFactoryTemplateDoubleHasBeenRegistered = true;
    MatlabTransformIOFactoryTemplate<double>::RegisterOneFactory();
    }
  }

} // end namespace itk
