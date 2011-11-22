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
#ifndef __itkCompensatedSummation_hxx
#define __itkCompensatedSummation_hxx

#include "itkNumericTraits.h"
#include "itkCompensatedSummation.h"

namespace itk
{

template < class TFloat >
CompensatedSummation< TFloat >
::CompensatedSummation():
  m_Sum( NumericTraits< AccumulateType >::Zero ),
  m_Compensation( NumericTraits< AccumulateType >::Zero )
{
}

template < class TFloat >
CompensatedSummation< TFloat >
::CompensatedSummation( const Self & rhs )
{
  this->m_Sum = rhs.m_Sum;
  this->m_Compensation = rhs.m_Compensation;
}

template < class TFloat >
void
CompensatedSummation< TFloat >
::operator=( const Self & rhs )
{
  if ( this == &rhs )
    {
    return;
    }

  this->m_Sum = rhs.m_Sum;
  this->m_Compensation = rhs.m_Compensation;
}

// For the Intel compiler
#ifdef __INTEL_COMPILER
#pragma optimize("", off)
#endif // __INTEL_COMPILER
template< class TFloat >
void
CompensatedSummation< TFloat >
::AddElement( const FloatType & input )
{
  const AccumulateType compensatedInput = static_cast< AccumulateType >( input - this->m_Compensation );
  const AccumulateType tempSum          = this->m_Sum + compensatedInput;
  // Warning: watch out for the compiler optimizing this out!
  this->m_Compensation                  = (tempSum - this->m_Sum) - compensatedInput;
  this->m_Sum                           = tempSum;
}
#ifdef __INTEL_COMPILER
#pragma optimize("", on)
#endif // __INTEL_COMPILER

template< class TFloat >
CompensatedSummation< TFloat > &
CompensatedSummation< TFloat >
::operator+=( const FloatType & rhs )
{
  this->AddElement( rhs );
  return *this;
}

template< class TFloat >
CompensatedSummation< TFloat > &
CompensatedSummation< TFloat >
::operator-=( const FloatType & rhs )
{
  this->AddElement( - rhs );
  return *this;
}

template< class TFloat >
CompensatedSummation< TFloat > &
CompensatedSummation< TFloat >
::operator*=( const FloatType & rhs )
{
  this->m_Sum *= rhs;
  this->m_Compensation *= rhs;
  return *this;
}

template< class TFloat >
CompensatedSummation< TFloat > &
CompensatedSummation< TFloat >
::operator/=( const FloatType & rhs )
{
  this->m_Sum /= rhs;
  this->m_Compensation /= rhs;
  return *this;
}

template< class TFloat >
void
CompensatedSummation< TFloat >
::ResetToZero()
{
  this->m_Sum          = NumericTraits< AccumulateType >::Zero;
  this->m_Compensation = NumericTraits< AccumulateType >::Zero;
}

template< class TFloat >
const typename CompensatedSummation< TFloat >::AccumulateType &
CompensatedSummation< TFloat >
::GetSum() const
{
  return this->m_Sum;
}

} // end namespace itk

#endif
