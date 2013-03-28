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
#ifndef __itkObjectToObjectMetricBase_h
#define __itkObjectToObjectMetricBase_h

#include "itkObjectToObjectMetricBaseTemplate.h"

namespace itk
{

/** \class ObjectToObjectMetricBase
 * \brief Base class for all object-to-object similarlity metrics added in ITKv4.
 *
 * This is the abstract base class for a hierarchy of similarity metrics
 * that may, in derived classes, operate on meshes, images, etc.
 * This class computes a value that measures the similarity between the two
 * objects.
 *
 * Derived classes must provide implementations for:
 *  GetValue
 *  GetDerivative
 *  GetValueAndDerivative
 *  Initialize
 *  GetNumberOfParameters
 *  GetNumberOfLocalParameters
 *  GetParameters
 *  SetParameters
 *  HasLocalSupport
 *  UpdateTransformParameters
 *
 * \ingroup ITKOptimizersv4
 */
typedef ObjectToObjectMetricBaseTemplate<double> ObjectToObjectMetricBase;

} // end namespace itk

#endif
