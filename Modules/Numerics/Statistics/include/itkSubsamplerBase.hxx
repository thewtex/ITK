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
#ifndef __itkSubsamplerBase_hxx
#define __itkSubsamplerBase_hxx
#include "itkSubsamplerBase.h"

namespace itk {
namespace Statistics {

template <class TSample>
SubsamplerBase<TSample>
::SubsamplerBase()
{
  m_Sample = NULL;
  m_RequestMaximumNumberOfResults = true;
  m_CanSelectQuery = true;
  m_Seed =  0;
}

template <class TSample>
typename SubsamplerBase<TSample>::Baseclass*
SubsamplerBase<TSample>
::Copy(const Baseclass* other)
{
  this->SetSample(other->GetSample());
  this->m_RequestMaximumNumberOfResults = other->m_RequestMaximumNumberOfResults;
  this->m_CanSelectQuery = other->m_CanSelectQuery;
  this->SetSeed(other->m_Seed);
  return this;
}

template <class TSample>
void
SubsamplerBase<TSample>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Request maximum number of results: " << m_RequestMaximumNumberOfResults
     << std::endl;
  os << indent << "Can select query index during search: " << m_CanSelectQuery << std::endl;
  os << indent << "seed: " << m_Seed << std::endl;

  if (m_Sample)
  {
    os << indent << "Sample: " << m_Sample << std::endl;
  }
  else
  {
    os << indent << "Sample is NULL" << std::endl;
  }

  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
