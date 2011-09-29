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
#ifndef __itkSpatialNeighborSubsampler_hxx
#define __itkSpatialNeighborSubsampler_hxx
#include "itkSpatialNeighborSubsampler.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk {
namespace Statistics {

template <class TSample, class TRegion>
typename SpatialNeighborSubsampler<TSample, TRegion>::Baseclass::Pointer
SpatialNeighborSubsampler<TSample, TRegion>
::Clone() const
{
  typename Baseclass::Pointer subsampler = Self::New().GetPointer()->Copy(this);
  return subsampler;
}

template <class TSample, class TRegion>
typename SpatialNeighborSubsampler<TSample, TRegion>::Baseclass*
SpatialNeighborSubsampler<TSample, TRegion>
::Copy(const Baseclass* other)
{
  const Self* otherPtr = dynamic_cast<const Self*>(other);
  this->Superclass::Copy(other);
  if (otherPtr->GetRadiusInitialized())
  {
    this->SetRadius(otherPtr->GetRadius());
  }
  else
  {
    this->m_RadiusInitialized = false;
  }
  if (otherPtr->GetSampleRegionInitialized())
  {
    this->SetSampleRegion(otherPtr->GetSampleRegion());
  }
  else
  {
    this->m_SampleRegionInitialized = false;
  }
  return this;
}

template <class TSample, class TRegion>
void
SpatialNeighborSubsampler<TSample, TRegion>
::SetSampleRegion(const RegionType& region)
{
  itkDebugMacro("setting sample region to " << region);
  if (this->m_SampleRegion != region ||
      !(this->m_SampleRegionInitialized))
  {
    this->m_SampleRegion = region;
    this->m_SampleRegionInitialized = true;
    this->Modified();
  }
}

template <class TSample, class TRegion>
void
SpatialNeighborSubsampler<TSample, TRegion>
::SetRadius(const RadiusType& radius)
{
  itkDebugMacro("Setting Radius to " << radius);
  if (this->m_Radius != radius ||
      !this->m_RadiusInitialized)
  {
    this->m_Radius = radius;
    this->m_RadiusInitialized = true;
    this->Modified();
  }
}

template <class TSample, class TRegion>
void
SpatialNeighborSubsampler<TSample, TRegion>
::SetRadius(unsigned int radius)
{
  RadiusType radiusND;
  radiusND.Fill(radius);
  this->SetRadius(radiusND);
}

template <class TSample, class TRegion>
void
SpatialNeighborSubsampler<TSample, TRegion>
::Search(const InstanceIdentifier& query,
         SubsamplePointer& results)
{
  if (!m_RadiusInitialized)
  {
    itkExceptionMacro(<< "Radius not set.");
  }
  if (!m_SampleRegionInitialized)
  {
    itkExceptionMacro(<< "Sample region not set.");
  }
  if (!this->GetRegionConstraintInitialized())
  {
    this->SetRegionConstraint(this->m_SampleRegion);
  }

  results->Clear();
  results->SetSample(this->m_Sample);

  RegionType searchRegion;
  IndexType searchIndex;
  SizeType searchSize;

  IndexType constraintIndex = this->m_RegionConstraint.GetIndex();
  SizeType constraintSize = this->m_RegionConstraint.GetSize();

  IndexType queryIndex;
  typename RegionType::OffsetTableType offsetTable;
  m_SampleRegion.ComputeOffsetTable(offsetTable);
  ImageHelperType::ComputeIndex(m_SampleRegion.GetIndex(),
                                query,
                                offsetTable,
                                queryIndex);

  for (unsigned int dim = 0; dim < RegionType::ImageDimension; ++dim)
  {
    if (queryIndex[dim] < static_cast<typename IndexType::IndexValueType>(m_Radius[dim]))
    {
      searchIndex[dim] = vnl_math_max(0l, constraintIndex[dim]);
    }
    else
    {
      searchIndex[dim] = vnl_math_max(static_cast<long int>(queryIndex[dim] - m_Radius[dim]),
                                      constraintIndex[dim]);
    }

    if (queryIndex[dim] + m_Radius[dim] < constraintIndex[dim] + constraintSize[dim])
    {
      searchSize[dim] = queryIndex[dim] + m_Radius[dim] - searchIndex[dim] + 1;
    }
    else
    {
      searchSize[dim] = (constraintIndex[dim] + constraintSize[dim]) - searchIndex[dim];
    }
  }

  searchRegion.SetIndex(searchIndex);
  searchRegion.SetSize(searchSize);

  if (!this->m_RegionConstraint.IsInside(queryIndex))
  {
    itkWarningMacro( "query point (" << query << ") corresponding to index ("
                     << queryIndex << ") is not inside the given image region ("
                     << this->m_RegionConstraint
                     << ").  No matching points found.");
    return;
  }

  IndexType endIndex = searchRegion.GetUpperIndex();
  IndexType positionIndex = searchIndex;
  bool someRemaining = true;
  typename RegionType::OffsetValueType offset = 0;

  // The trouble with decoupling the region from the sample is that
  // there is an implicit assumption that the instance identifiers in the sample
  // are ordered as if someone was iterating forward through the region
  // TODO Is this a safe assumption to make?

  if (this->m_CanSelectQuery ||
      (positionIndex != queryIndex))
  {
    offset = 0;
    ImageHelperType::ComputeOffset(m_SampleRegion.GetIndex(),
                                   positionIndex,
                                   offsetTable,
                                   offset);
    results->AddInstance(static_cast<InstanceIdentifier>(offset));
  }

  while (someRemaining)
  {
    someRemaining = false;
    for (unsigned int dim = 0; dim < ImageDimension; ++dim)
    {
      positionIndex[dim]++;
      if ( positionIndex[dim] < endIndex[dim] )
      {
        offset += offsetTable[dim];
        if (this->m_CanSelectQuery ||
            (static_cast<InstanceIdentifier>(offset) != query))
        {
          results->AddInstance(static_cast<InstanceIdentifier>(offset));
        }
        someRemaining = true;
        break;
      }
      else
      {
        offset -= offsetTable[dim] * (static_cast< typename RegionType::OffsetValueType >(searchSize[dim]) - 1);
        positionIndex[dim] = searchIndex[dim];
      }
    }
  }
} // end Search method

template <class TSample, class TRegion>
void
SpatialNeighborSubsampler<TSample, TRegion>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  if (m_RadiusInitialized)
  {
    os << indent << "Radius initialized as: " << m_Radius
     << std::endl;
  }
  else
  {
    os << indent << "Radius not initialized yet." << std::endl;
  }

  if (m_SampleRegionInitialized)
  {
    os << indent << "Sample region initialized as: "
       << m_SampleRegion << std::endl;
  }
  else
  {
    os << indent << "Sample region not initialized yet." << std::endl;
  }
  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
