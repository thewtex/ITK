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
#ifndef __itkTxtTransformIOFactory_hxx
#define __itkTxtTransformIOFactory_hxx

#include "itkTxtTransformIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkTxtTransformIO.h"
#include "itkVersion.h"

namespace itk
{
template<class ParametersValueType>
void
TxtTransformIOFactoryTemplate<ParametersValueType>
::PrintSelf(std::ostream &, Indent) const
{}

template<class ParametersValueType>
TxtTransformIOFactoryTemplate<ParametersValueType>
::TxtTransformIOFactoryTemplate()
{
  this->RegisterOverride( "itkTransformIOBaseTemplate",
                          "itkTxtTransformIOTemplate",
                          "Txt Transform IO Template",
                          1,
                          CreateObjectFunction< TxtTransformIOTemplate<ParametersValueType> >::New() );
}

template<class ParametersValueType>
TxtTransformIOFactoryTemplate<ParametersValueType>
::~TxtTransformIOFactoryTemplate()
{}

template<class ParametersValueType>
const char *
TxtTransformIOFactoryTemplate<ParametersValueType>
::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

template<class ParametersValueType>
const char *
TxtTransformIOFactoryTemplate<ParametersValueType>
::GetDescription() const
{
  return "Txt TransformIO Factory, allows the"
         " loading of Nifti images into insight";
}
} // end namespace itk

#endif
