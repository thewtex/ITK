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
#include "itkNumericTraits.h"
#include "itkCompensatedSummation.h"

namespace itk
{

template< class TFloat >
void
CompensatedSummation< TFloat >
::AddElement( const FloatType & nextTerm )
{
  const AccumulateType compensatedInput = static_cast< AccumulateType >( nextTerm - this->m_Compensation );
  const AccumulateType tempSum          = this->m_Sum + compensatedInput;
  // Warning: watch out for the compiler optimizing this out!
  this->m_Compensation                  = (tempSum - this->m_Sum) - compensatedInput;
  this->m_Sum                           = tempSum;
}

template class CompensatedSummation< float >;
template class CompensatedSummation< double >;

} // end namespace itk
