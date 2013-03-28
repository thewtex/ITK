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
#ifndef __itkMatlabTransformIOFactory_hxx
#define __itkMatlabTransformIOFactory_hxx

#include "itkMatlabTransformIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkMatlabTransformIO.h"
#include "itkVersion.h"

namespace itk
{
template<class ParametersValueType>
void
MatlabTransformIOFactoryTemplate<ParametersValueType>
::PrintSelf(std::ostream &, Indent) const
{}

template<class ParametersValueType>
MatlabTransformIOFactoryTemplate<ParametersValueType>
::MatlabTransformIOFactoryTemplate()
{
  this->RegisterOverride( "itkTransformIOBaseTemplate",
                          "itkMatlabTransformIOTemplate",
                          "Matlab Transform IO Template",
                          1,
                          CreateObjectFunction< MatlabTransformIOTemplate<ParametersValueType> >::New() );
}

template<class ParametersValueType>
MatlabTransformIOFactoryTemplate<ParametersValueType>
::~MatlabTransformIOFactoryTemplate()
{}

template<class ParametersValueType>
const char *
MatlabTransformIOFactoryTemplate<ParametersValueType>
::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

template<class ParametersValueType>
const char *
MatlabTransformIOFactoryTemplate<ParametersValueType>
::GetDescription() const
{
  return "Matlab TransformIO Factory, allows the "
         "loading of Nifti images into insight";
}
} // end namespace itk

#endif
