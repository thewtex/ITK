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
#ifndef __itkHDF5TransformIOFactoryTemplate_hxx
#define __itkHDF5TransformIOFactoryTemplate_hxx

#include "itkHDF5TransformIOFactoryTemplate.h"
#include "itkCreateObjectFunction.h"
#include "itkHDF5TransformIOTemplate.h"
#include "itkVersion.h"

namespace itk
{
template<class ParametersValueType>
void
HDF5TransformIOFactoryTemplate<ParametersValueType>
::PrintSelf(std::ostream &, Indent) const
{}

template<class ParametersValueType>
HDF5TransformIOFactoryTemplate<ParametersValueType>
::HDF5TransformIOFactoryTemplate()
{
  this->RegisterOverride( "itkTransformIOBaseTemplate",
                          "itkHDF5TransformIOTemplate",
                          "HDF5 Transform IO Template",
                          1,
                          CreateObjectFunction< itk::HDF5TransformIOTemplate<ParametersValueType> >::New() );
}

template<class ParametersValueType>
HDF5TransformIOFactoryTemplate<ParametersValueType>
::~HDF5TransformIOFactoryTemplate()
{}

template<class ParametersValueType>
const char *
HDF5TransformIOFactoryTemplate<ParametersValueType>
::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

template<class ParametersValueType>
const char *
HDF5TransformIOFactoryTemplate<ParametersValueType>
::GetDescription() const
{
  return "HD5 TransformIO Factory, allows the"
         " loading of HDF5 transforms into insight";
}
} // end namespace itk

extern template class itk::HDF5TransformIOFactoryTemplate<float>;
extern template class itk::HDF5TransformIOFactoryTemplate<double>;

#endif
