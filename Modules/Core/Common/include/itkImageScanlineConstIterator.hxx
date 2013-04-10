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
#ifndef __itkImageScanlineConstIterator_hxx
#define __itkImageScanlineConstIterator_hxx

#include "itkImageScanlineConstIterator.h"

namespace itk
{

//----------------------------------------------------------------------------
// Increment when the fastest moving direction has reached its bound.
// This method should *ONLY* be invoked from the operator++() method.
template< class TImage >
void
ImageScanlineConstIterator< TImage >
::Increment()
{
  // We have reached the end of the span (row), need to wrap around.

  // Get the index of the last pixel on the span (row)
  IndexType ind = this->m_Image->ComputeIndex( static_cast< OffsetValueType >( m_SpanEndOffset -1 ) );

  const IndexType &startIndex = this->m_Region.GetIndex();
  const SizeType &size = this->m_Region.GetSize();

  // Check to see if this is the last row
  if (ind == this->m_Region.GetUpperIndex())
    {
    return;
    }

  // Note that ++ind[0] moves to the next pixel after span end
  ++ind[0];

  // if the iterator is outside the region (but not past region end) then
  // we need to wrap around the region
  for( unsigned int d = 0; d+1 < ImageIteratorDimension; ++d)
    {
    if ( static_cast< SizeValueType >( ind[d] - startIndex[d] ) >= size[d] )
      {
      ind[d] = startIndex[d];
      ++ind[d+1];
      }
    }

  this->m_Offset = this->m_Image->ComputeOffset(ind);
  m_SpanEndOffset = this->m_Offset + static_cast< OffsetValueType >( size[0] );
  m_SpanBeginOffset = this->m_Offset;
}

} // end namespace itk

#endif
