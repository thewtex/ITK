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
#include "itkHD5TransformIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkHD5TransformIO.h"
#include "itkVersion.h"

namespace itk
{
void HD5TransformIOFactory::PrintSelf(std::ostream &, Indent) const
{}

HD5TransformIOFactory::HD5TransformIOFactory()
{
  this->RegisterOverride( "itkTransformIOBase",
                          "itkHD5TransformIO",
                          "HD5 Transform IO",
                          1,
                          CreateObjectFunction< HD5TransformIO >::New() );
}

HD5TransformIOFactory::~HD5TransformIOFactory()
{}

const char *
HD5TransformIOFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char *
HD5TransformIOFactory::GetDescription() const
{
  return "HD5 TransformIO Factory, allows the"
         " loading of HDF5 transforms into insight";
}
} // end namespace itk
