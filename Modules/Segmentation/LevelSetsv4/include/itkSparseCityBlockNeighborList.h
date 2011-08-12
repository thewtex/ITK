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

#ifndef __itkSparseCityBlockNeighborList_h
#define __itkSparseCityBlockNeighborList_h

#include "itkMacro.h"
#include <vector>

namespace itk
{
/**
 * \class SparseCityBlockNeighborList
 *
 * \brief A convenience class for storing indicies which reference neighbor
 * pixels within a neighborhood.
 *
 * \par
 * This class creates and stores indicies for use in finding neighbors within
 * an NeighborhoodIterator object. Both an array of unsigned integer
 * indicies and an array of N dimensional offsets (from the center of the
 * neighborhood) are created and stored. The indicies and offsets correspond
 * to the "city-block" neighbors, that is, 4-neighbors in 2d, 6-neighbors in
 * 3d, etc.
 *
 * \par
 * Order of reference is lowest index to highest index in the neighborhood.
 * For example, for 4 connectivity, the indicies refer to the following
 * neighbors:
 *
 * \verbatim
 *
 *  * 1 *
 *  2 * 3
 *  * 4 *
 *
 * \endverbatim
 *
 * \ingroup ITKLevelSetsv4
 */
template< class TNeighborhood >
class SparseCityBlockNeighborList
{
public:
  typedef TNeighborhood                               NeighborhoodType;
  typedef typename NeighborhoodType::OffsetType       OffsetType;
  typedef typename NeighborhoodType::OffsetValueType  OffsetValueType;
  typedef typename NeighborhoodType::RadiusType       RadiusType;

  itkStaticConstMacro(Dimension, unsigned int,
                      NeighborhoodType::Dimension);

  /** Make sure the layers are a single pixel thick */
  const RadiusType & GetRadius() const
  { return m_Radius; }

  const unsigned int & GetArrayIndex(unsigned int i) const
  { return m_ArrayIndex[i]; }

  const OffsetType & GetNeighborhoodOffset(unsigned int i) const
  { return m_NeighborhoodOffset[i]; }

  const unsigned int & GetSize() const
  { return m_Size; }

  OffsetValueType GetStride(unsigned int i) const
  { return m_StrideTable[i]; }

  SparseCityBlockNeighborList() : m_Size( 2 * Dimension ),
      m_ArrayIndex( 2 * Dimension ),
      m_NeighborhoodOffset( 2 * Dimension )
    {
    typedef typename NeighborhoodType::ImageType ImageType;
    typename ImageType::Pointer dummy_image = ImageType::New();

    unsigned int i;
    int          d;
    OffsetType   zero_offset;

    for ( i = 0; i < Dimension; ++i )
      {
      m_Radius[i] = 1;
      zero_offset[i] = 0;
      }
    NeighborhoodType it( m_Radius, dummy_image, dummy_image->GetRequestedRegion() );
    unsigned int nCenter = it.Size() / 2;

//    m_Size = 2 * Dimension;
//    m_ArrayIndex.reserve(m_Size);
//    m_NeighborhoodOffset.reserve(m_Size);

    for ( i = 0; i < m_Size; ++i )
      {
      m_NeighborhoodOffset.push_back(zero_offset);
      }

    for ( d = static_cast< int >( Dimension ) - 1, i = 0; d >= 0; --d, ++i )
      {
      m_ArrayIndex.push_back( nCenter - it.GetStride(d) );
      m_NeighborhoodOffset[i][d] = -1;
      }
    for ( i = 0; i < Dimension; ++i )
      {
      m_ArrayIndex.push_back( nCenter + it.GetStride(i) );
      m_NeighborhoodOffset[i][i] = 1;
      m_StrideTable[i] = it.GetStride(i);
      }
    }
  ~SparseCityBlockNeighborList() {}

  void Print(std::ostream & os) const
    {
    os << "SparseCityBlockNeighborList: " << std::endl;
    for ( unsigned int i = 0; i < m_Size; ++i )
      {
      os << "m_ArrayIndex[" << i << "]: " << m_ArrayIndex[i] << std::endl;
      os << "m_NeighborhoodOffset[" << i << "]: " << m_NeighborhoodOffset[i]
         << std::endl;
      }
    }

private:
  unsigned int                m_Size;
  RadiusType                  m_Radius;
  std::vector< unsigned int > m_ArrayIndex;
  std::vector< OffsetType >   m_NeighborhoodOffset;

  /** An internal table for keeping track of stride lengths in a neighborhood,
      i.e. the memory offsets between pixels along each dimensional axis. */
  OffsetValueType m_StrideTable[Dimension];
};
}

#endif // __itkSparseCityBlockNeighborList_h
