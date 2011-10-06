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
#ifndef __itkThreadedDomainPartitioner_hxx
#define __itkThreadedDomainPartitioner_hxx

#include "itkThreadedDomainPartitioner.h"
#include "vnl/vnl_math.h"

namespace itk
{

template <class TDomain >
ThreadedDomainPartitioner< TDomain >
::ThreadedDomainPartitioner()
{
}

template <class TDomain >
ThreadedDomainPartitioner< TDomain >
::~ThreadedDomainPartitioner()
{
}

} // end namespace itk

#endif
