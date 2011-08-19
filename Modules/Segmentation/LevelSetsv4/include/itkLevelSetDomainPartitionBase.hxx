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
#ifndef __itkLevelSetDomainPartitionBase_hxx
#define __itkLevelSetDomainPartitionBase_hxx

#include "itkLevelSetDomainPartitionBase.h"

namespace itk
{
template< class TDomain >
void
LevelSetDomainPartitionBase< TDomain >::
SetFunctionCount(const IdentifierType & n)
{
  this->m_FunctionCount = n;
}


template< class TDomain >
typename itk::IdentifierType
LevelSetDomainPartitionBase< TDomain >::
GetFunctionCount() const
{
  return this->m_FunctionCount;
}

template< class TDomain >
LevelSetDomainPartitionBase< TDomain >::
LevelSetDomainPartitionBase(): m_FunctionCount( 1 )
{
}

template< class TDomain >
LevelSetDomainPartitionBase< TDomain >::
~LevelSetDomainPartitionBase()
{
}

} //end namespace itk

#endif
