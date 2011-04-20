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
#ifndef __itkPeriodicBoundaryCondition_hpp
#define __itkPeriodicBoundaryCondition_hpp
#include "itkConstNeighborhoodIterator.h"
#include "itkPeriodicBoundaryCondition.h"
namespace itk
{
template< class TImage >
typename PeriodicBoundaryCondition< TImage >::PixelType
PeriodicBoundaryCondition< TImage >
::operator()(const OffsetType & point_index, const OffsetType & boundary_offset,
             const NeighborhoodType *data) const
{
  typedef typename OffsetType::OffsetValueType OffsetValueType;
  const ConstNeighborhoodIterator< TImage > *iterator =
    dynamic_cast< const ConstNeighborhoodIterator< TImage > * >( data );
  typename TImage::PixelType * ptr;
  int          linear_index = 0;
  unsigned int i;

  // Find the pointer of the closest boundary pixel

  // Return the value of the pixel at the closest boundary point.
  for ( i = 0; i < ImageDimension; ++i )
    {
    linear_index += ( point_index[i] + boundary_offset[i] ) * data->GetStride(i);
    }
  ptr = data->operator[](linear_index);

  // Wrap the pointer around the image in the necessary dimensions.  If we have
  // reached this point, we can assume that we are on the edge of the BUFFERED
  // region of the image.  Boundary conditions are only invoked if touching the
  // actual memory boundary.

  // These are the step sizes for increments in each dimension of the image.
  const typename TImage::OffsetValueType * offset_table =
    iterator->GetImagePointer()->GetOffsetTable();

  for ( i = 0; i < ImageDimension; ++i )
    {
    if ( boundary_offset[i] != 0 )
      { // If the neighborhood overlaps on the low edge, then wrap from the
        // high edge of the image.
      if ( point_index[i] < static_cast< OffsetValueType >( iterator->GetRadius(i) ) )
        {
        ptr += iterator->GetImagePointer()->GetBufferedRegion().GetSize()[i]
               * offset_table[i] - boundary_offset[i] * offset_table[i];
        }
      else // wrap from the low side of the image
        {
        ptr -= iterator->GetImagePointer()->GetBufferedRegion().GetSize()[i]
               * offset_table[i] + boundary_offset[i] * offset_table[i];
        }
      }
    }

  return *ptr;
}

template< class TImage >
typename PeriodicBoundaryCondition< TImage >::PixelType
PeriodicBoundaryCondition< TImage >
::operator()(const OffsetType & point_index, const OffsetType & boundary_offset,
             const NeighborhoodType *data,
             const NeighborhoodAccessorFunctorType & neighborhoodAccessorFunctor) const
{
  typedef typename OffsetType::OffsetValueType OffsetValueType;
  const ConstNeighborhoodIterator< TImage > *iterator =
    dynamic_cast< const ConstNeighborhoodIterator< TImage > * >( data );
  typename TImage::PixelType * ptr;
  int          linear_index = 0;
  unsigned int i;

  // Find the pointer of the closest boundary pixel
  //  std::cout << "Boundary offset = " << boundary_offset << std::endl;
  // std::cout << "point index = " << point_index << std::endl;

  // Return the value of the pixel at the closest boundary point.
  for ( i = 0; i < ImageDimension; ++i )
    {
    linear_index += ( point_index[i] + boundary_offset[i] ) * data->GetStride(i);
    }
  ptr = data->operator[](linear_index);

  // Wrap the pointer around the image in the necessary dimensions.  If we have
  // reached this point, we can assume that we are on the edge of the BUFFERED
  // region of the image.  Boundary conditions are only invoked if touching the
  // actual memory boundary.

  // These are the step sizes for increments in each dimension of the image.
  const typename TImage::OffsetValueType * offset_table =
    iterator->GetImagePointer()->GetOffsetTable();

  for ( i = 0; i < ImageDimension; ++i )
    {
    if ( boundary_offset[i] != 0 )
      { // If the neighborhood overlaps on the low edge, then wrap from the
        // high edge of the image.
      if ( point_index[i] < static_cast< OffsetValueType >( iterator->GetRadius(i) ) )
        {
        ptr += iterator->GetImagePointer()->GetBufferedRegion().GetSize()[i]
               * offset_table[i] - boundary_offset[i] * offset_table[i];
        }
      else // wrap from the low side of the image
        {
        ptr -= iterator->GetImagePointer()->GetBufferedRegion().GetSize()[i]
               * offset_table[i] + boundary_offset[i] * offset_table[i];
        }
      }
    }

  return neighborhoodAccessorFunctor.Get(ptr);
}
} // end namespace itk

#endif
