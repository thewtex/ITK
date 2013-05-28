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
#include "itkThreadedIndexedContainerPartitioner.h"
#include "vnl/vnl_math.h"

namespace itk
{

ThreadedIndexedContainerPartitioner
::ThreadedIndexedContainerPartitioner()
{
}

ThreadedIndexedContainerPartitioner
::~ThreadedIndexedContainerPartitioner()
{
}

ThreadIdType
ThreadedIndexedContainerPartitioner
::PartitionDomain( const ThreadIdType threadId,
                        const ThreadIdType requestedTotal,
                        const DomainType& completeIndexRange,
                        DomainType& subIndexRange) const
{
  // Protect against division by 0 below. Seems this would be a bug
  // in MultiThreader if it passed 0 for requestedTotal.
  if( requestedTotal == 0 )
    {
    itkExceptionMacro( "requestedTotal == 0. "
                      << "Error in input 'requestedTotal'" );
    }

  // Determine the actual number of pieces that will be generated
  const DomainLengthType count = static_cast<double>( completeIndexRange[1] - completeIndexRange[0] + 1 );

  // Make sure we don't have a 0-valued count to avoid
  // division by 0 below.
  // This would be a bug in the passed completeIndexRange.
  if( count == 0 )
    {
    itkExceptionMacro( "completeIndexRange[1] - completeIndexRange[0] + 1 == 0. "
                      << "Error: input 'completeIndexRange' is zero-length." );
    }

  DomainLengthType valuesPerThread =
    Math::Ceil<DomainLengthType>( static_cast< double >( count ) / static_cast< double >( requestedTotal ));
  ThreadIdType maxThreadIdUsed =
    Math::Ceil<ThreadIdType>( static_cast< double >( count ) / static_cast< double >( valuesPerThread )) - 1;

  // Split the index range
  if (threadId < maxThreadIdUsed)
    {
    subIndexRange[0] = completeIndexRange[0] + threadId * valuesPerThread;
    subIndexRange[1] = subIndexRange[0] + valuesPerThread - 1;
    }
  else if (threadId == maxThreadIdUsed)
    {
    subIndexRange[0] = completeIndexRange[0] + threadId * valuesPerThread;
    // Last thread needs to process the "rest" of the range
    subIndexRange[1] = completeIndexRange[1];
    }
  else // if( threadId > maxThreadIdUsed )
    {
    // To be safe, for a threadId past the maxThreadIdUsed,
    // subIndexRange should be zero-length at the end of the completeIndexRange
    // However, the semantics of ThreadedIndexedContainerPartitioner::DomainType
    // don't provide a clean way to specify zero-length, so just set both to -1
    // even though this could be interpreted as having a length of 1
    subIndexRange[0] = -1;
    subIndexRange[1] = -1;
    }

  itkDebugMacro("ThreadedIndexedContainerPartitioner:  Split : " << subIndexRange );

  return maxThreadIdUsed + 1;
}

} // end namespace itk
