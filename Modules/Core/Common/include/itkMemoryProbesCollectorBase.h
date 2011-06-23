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
#ifndef __itkMemoryProbesCollectorBase_h
#define __itkMemoryProbesCollectorBase_h

#include "itkMacro.h"
#include "itkMemoryProbe.h"
#include "itkResourceProbesCollectorBase.h"

namespace itk
{
/** \class MemoryProbesCollectorBase
 *  \brief Class for aggregating a set of memory probes.
 *
 *  This class defines a set of MemoryProbes and assign names to them.
 *  The user can start and stop each one of the probes by addressing them by name.
 *
 *  \sa MemoryProbe
 *
 * \ingroup ITK_Common
 */
class ITK_EXPORT MemoryProbesCollectorBase:public ResourceProbesCollectorBase< MemoryProbe >
{
public:
  virtual ~MemoryProbesCollectorBase(){}
};
} // end namespace itk

#endif // __itkMemoryProbesCollectorBase_h
