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
#ifndef __itkGradientDescentObjectOptimizerBaseModifyGradientByLearningRateThreader_hxx
#define __itkGradientDescentObjectOptimizerBaseModifyGradientByLearningRateThreader_hxx

#include "itkGradientDescentObjectOptimizerBaseModifyGradientByLearningRateThreader.h"

namespace itk
{

template< class TGradientDescentObjectOptimizer >
void
GradientDescentObjectOptimizerBaseModifyGradientByLearningRateThreader< TGradientDescentObjectOptimizer >
::ThreadedExecution( const IndexRangeType & subrange,
                     const ThreadIdType itkNotUsed(threadId) )
{
  this->m_Associate->ModifyGradientByLearningRateOverSubRange( subrange );
}

} // end namespace itk

#endif
