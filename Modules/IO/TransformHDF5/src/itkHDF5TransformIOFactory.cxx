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
#include "itkHDF5TransformIOFactory.h"

template class itk::HDF5TransformIOFactoryTemplate<float>;
template class itk::HDF5TransformIOFactoryTemplate<double>;

namespace itk
{
// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.
static bool HDF5TransformIOFactoryTemplateFloatHasBeenRegistered = false;
void HDF5TransformIOFactoryTemplateFloatRegister__Private(void)
{
if( ! HDF5TransformIOFactoryTemplateFloatHasBeenRegistered )
  {
  HDF5TransformIOFactoryTemplateFloatHasBeenRegistered = true;
  HDF5TransformIOFactoryTemplate<float>::RegisterOneFactory();
  }
}

static bool HDF5TransformIOFactoryTemplateDoubleHasBeenRegistered = false;
void HDF5TransformIOFactoryTemplateDoubleRegister__Private(void)
{
if( ! HDF5TransformIOFactoryTemplateDoubleHasBeenRegistered )
  {
  HDF5TransformIOFactoryTemplateDoubleHasBeenRegistered = true;
  HDF5TransformIOFactoryTemplate<double>::RegisterOneFactory();
  }
}
} // end namespace itk
