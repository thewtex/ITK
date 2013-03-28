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
#ifndef __itkHDF5TransformIO_h
#define __itkHDF5TransformIO_h

#include "itkHDF5TransformIOTemplate.h"

namespace itk
{
/** \class HDF5TransformIO
 *  \brief Read&Write transforms in HDF5 Format
 *
 *  See hdfgroup.org/HDF5 -- HDF5 is a physics/astrophysics
 *  format, but it is very general and can store pretty much
 *  any sort of data.
 *
 * \ingroup ITKIOTransformHDF5
 */
typedef HDF5TransformIOTemplate<double> HDF5TransformIO;

}
#endif // __itkHDF5TransformIO_h
