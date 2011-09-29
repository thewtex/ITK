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
#ifndef __itkUniformRandomSpatialNeighborSubsampler_hxx
#define __itkUniformRandomSpatialNeighborSubsampler_hxx
#include "itkUniformRandomSpatialNeighborSubsampler.h"
#include <set>

namespace itk {
namespace Statistics {

template <class TSample>
typename UniformRandomSpatialNeighborSubsampler<TSample>::Baseclass::Pointer
UniformRandomSpatialNeighborSubsampler<TSample>
::Clone() const
{
  typename Baseclass::Pointer subsampler = Self::New().GetPointer()->Copy(this);
  return subsampler;
}

template <class TSample>
typename UniformRandomSpatialNeighborSubsampler<TSample>::Baseclass*
UniformRandomSpatialNeighborSubsampler<TSample>
::Copy(const Baseclass* other)
{
  const Self* otherPtr = dynamic_cast<const Self*>(other);
  this->Superclass::Copy(other);
  this->m_NumberOfResultsRequested = otherPtr->m_NumberOfResultsRequested;
  this->SetSeed(otherPtr->m_Seed);
  this->SetUseClockForSeed(otherPtr->m_UseClockForSeed);
  return this;
}

template <class TSample>
void
UniformRandomSpatialNeighborSubsampler<TSample>
::Search(const InstanceIdentifier& query,
         SubsamplePointer& results)
{
  if (!this->m_RadiusInitialized)
  {
    itkExceptionMacro(<< "Radius not set.");
  }
  results->Clear();
  results->SetSample(this->m_Sample);

  RegionType searchRegion;
  IndexType searchStartIndex;
  IndexType searchEndIndex;

  IndexType constraintIndex = this->m_RegionConstraint.GetIndex();
  SizeType constraintSize = this->m_RegionConstraint.GetSize();

  typename SampleType::NeighborhoodIteratorType queryNeighborhood;
  IndexType queryIndex;

  queryNeighborhood = this->m_Sample->GetMeasurementVector(query)[0];
  queryIndex = queryNeighborhood.GetIndex(queryNeighborhood.GetCenterNeighborhoodIndex());

  unsigned int numberOfPoints = 1;

  for (unsigned int dim = 0; dim < ImageType::ImageDimension; ++dim)
  {
    if (queryIndex[dim] < static_cast<typename IndexType::IndexValueType>(this->m_Radius[dim]))
    {
      searchStartIndex[dim] = vnl_math_max(0l, constraintIndex[dim]);
    }
    else
    {
      searchStartIndex[dim] = vnl_math_max(static_cast<long int>(queryIndex[dim] - this->m_Radius[dim]),
                                      constraintIndex[dim]);
    }

    if (queryIndex[dim] + this->m_Radius[dim] < constraintIndex[dim] + constraintSize[dim])
    {
      searchEndIndex[dim] = queryIndex[dim] + this->m_Radius[dim];
    }
    else
    {
      searchEndIndex[dim] = (constraintIndex[dim] + constraintSize[dim]) - 1;
    }
    numberOfPoints = numberOfPoints * (searchEndIndex[dim] - searchStartIndex[dim] + 1);
  }

  if (!this->m_RegionConstraint.IsInside(queryIndex))
  {
    itkWarningMacro( "query point (" << query << ") corresponding to index ("
                     << queryIndex << ") is not inside the given image region ("
                     << this->m_RegionConstraint
                     << ").  No matching points found.");
    return;
  }

  typename ImageType::ConstPointer image = this->m_Sample->GetImage();

  if (!this->m_RequestMaximumNumberOfResults &&
      (m_NumberOfResultsRequested < numberOfPoints))
  {
    numberOfPoints = m_NumberOfResultsRequested;
  }

  unsigned int pointsFound = 0;

  ::std::set<InstanceIdentifier> usedIds;

  // TODO find a cleaner way to optimize whether we need to check for
  // selecting query index than just duplicating the loops
  if (this->m_CanSelectQuery)
  {
    while (pointsFound < numberOfPoints)
    {
      // randomly select an index
      IndexType index;
      for (unsigned int dim = 0; dim < ImageType::ImageDimension; ++dim)
      {
        index[dim] = this->GetIntegerVariate(searchStartIndex[dim],
                                             searchEndIndex[dim],
                                             queryIndex[dim]);
      }
      InstanceIdentifier id = image->ComputeOffset(index);
      results->AddInstance(id);
      ++pointsFound;
    }
  }
  else
  {
    while (pointsFound < numberOfPoints)
    {
      // randomly select an index
      IndexType index;
      for (unsigned int dim = 0; dim < ImageType::ImageDimension; ++dim)
      {
        index[dim] = this->GetIntegerVariate(searchStartIndex[dim],
                                             searchEndIndex[dim],
                                             queryIndex[dim]);
      }
      // only include this index if it is not the query point
      if (index != queryIndex)
      {
        InstanceIdentifier id = image->ComputeOffset(index);
        results->AddInstance(id);
        ++pointsFound;
      }
    }
  }
} // end Search method

template <class TSample>
typename UniformRandomSpatialNeighborSubsampler<TSample>::RandomIntType
UniformRandomSpatialNeighborSubsampler<TSample>
::GetIntegerVariate(RandomIntType lowerBound,
                    RandomIntType upperBound,
                    RandomIntType itkNotUsed(mean))
{
  RandomIntType sizeRange = upperBound - lowerBound;
  // mean ignored since we are uniformly sampling
  return lowerBound +
    m_RandomNumberGenerator->GetIntegerVariate(sizeRange);
}

template <class TSample>
void
UniformRandomSpatialNeighborSubsampler<TSample>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of results requested: " << m_NumberOfResultsRequested << std::endl;
  os << indent << "Use clock for seed: " << m_UseClockForSeed << std::endl;
  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
