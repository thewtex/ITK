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
#ifndef __itkThreadedIteratorRangePartitioner_hxx
#define __itkThreadedIteratorRangePartitioner_hxx

#include "itkNumericTraits.h"
#include "itkThreadedIteratorRangePartitioner.h"
#include "itkMath.h"

namespace itk
{

template< class TIterator >
ThreadedIteratorRangePartitioner< TIterator >
::ThreadedIteratorRangePartitioner()
{
}

template< class TIterator >
ThreadedIteratorRangePartitioner< TIterator >
::~ThreadedIteratorRangePartitioner()
{
}

template< class TIterator >
ThreadIdType
ThreadedIteratorRangePartitioner< TIterator >
::PartitionDomain( const ThreadIdType threadId,
                   const ThreadIdType requestedTotal,
                   const DomainType& completeDomain,
                   DomainType& subDomain ) const
{
  // Protect against division by 0 below. Seems this would be a bug
  // in MultiThreader if it passed 0 for requestedTotal.
  if( requestedTotal == 0 )
    {
    itkExceptionMacro( "requestedTotal == 0. "
                      << "Error: input 'requestedTotal' is zero-length." );
    }

  // Determine the actual number of pieces that will be generated
  DomainIteratorType it;
  DomainLengthType count = 0;
  for( it = completeDomain.Begin(); it != completeDomain.End(); ++it )
    {
    ++count;
    }

  // Make sure we don't have a 0-valued count to avoid
  // division by 0 below.
  // This would be a bug in the passed completeDomain.
  if( count == 0 )
    {
    itkExceptionMacro( "completeDomain.Begin() == completeDomain.End(). "
                      << "Error in input 'completeDomain'" );
    }

  DomainLengthType valuesPerThread =
    Math::Ceil<DomainLengthType>( static_cast< double >( count ) / static_cast< double >( requestedTotal ));
  ThreadIdType maxThreadIdUsed =
    Math::Ceil<ThreadIdType>( static_cast< double >( count ) / static_cast< double >( valuesPerThread )) - 1;

  // Split the domain
  if( threadId > maxThreadIdUsed )
    {
    // To be safe, for a threadId past the maxThreadIdUsed,
    // subDomain should be zero-length at the end of the completeDomain
    subDomain.m_Begin = completeDomain.End();
    subDomain.m_End = completeDomain.End();
    }
  else // if( threadId <= maxThreadIdUsed )
    {
    it = completeDomain.Begin();
    const DomainLengthType startIndexCount = threadId * valuesPerThread;
    for( DomainLengthType ii = 0; ii < startIndexCount; ++ii )
      {
      ++it;
      }
    subDomain.m_Begin = it;

    if( threadId < maxThreadIdUsed )
      {
      const DomainLengthType endIndexCount = valuesPerThread;
      for( DomainLengthType ii = 0; ii < endIndexCount; ++ii )
        {
        ++it;
        }
      subDomain.m_End = it;
      }
    else // if( threadId == maxThreadIdUsed )
      {
      // Last thread needs to process the "rest" of the range
      subDomain.m_End = completeDomain.End();
      }
    }

  return maxThreadIdUsed + 1;
}

} // end namespace itk

#endif
