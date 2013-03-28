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
#ifndef __itkMatlabTransformIOFactory_h
#define __itkMatlabTransformIOFactory_h


#include "itkMatlabTransformIOFactoryTemplate.h"

namespace itk
{
/** \class MatlabTransformIOFactory
 *  \brief Create instances of MatlabTransformIO objects using an
 *  object factory.
 * \ingroup ITKIOTransformMatlab
 */
typedef MatlabTransformIOFactoryTemplate<double> MatlabTransformIOFactory;

} // end namespace itk

#endif
