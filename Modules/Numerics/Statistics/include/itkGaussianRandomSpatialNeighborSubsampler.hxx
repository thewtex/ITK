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
#ifndef __itkGaussianRandomSpatialNeighborSubsampler_hxx
#define __itkGaussianRandomSpatialNeighborSubsampler_hxx
#include "itkGaussianRandomSpatialNeighborSubsampler.h"

namespace itk {
namespace Statistics {

template <class TSample>
typename GaussianRandomSpatialNeighborSubsampler<TSample>::Baseclass::Pointer
GaussianRandomSpatialNeighborSubsampler<TSample>
::Clone() const
{
  typename Baseclass::Pointer subsampler = Self::New().GetPointer()->Copy(this);
  return subsampler;
}

template <class TSample>
typename GaussianRandomSpatialNeighborSubsampler<TSample>::Baseclass*
GaussianRandomSpatialNeighborSubsampler<TSample>
::Copy(const Baseclass* other)
{
  const Self* otherPtr = dynamic_cast<const Self*>(other);
  this->Superclass::Copy(other);
  this->m_Variance = otherPtr->m_Variance;
  return this;
}

template <class TSample>
typename GaussianRandomSpatialNeighborSubsampler<TSample>::RandomIntType
GaussianRandomSpatialNeighborSubsampler<TSample>
::GetIntegerVariate(RandomIntType lowerBound,
                    RandomIntType upperBound,
                    RandomIntType mean)
{
  assert(lowerBound <= upperBound);

  RandomIntType randInt = 0;

  do
  {
    RealType randVar = this->m_RandomNumberGenerator->GetNormalVariate(mean,
                                                                 m_Variance);
    randInt = static_cast<RandomIntType>(vcl_floor(randVar));
  } while ((randInt < lowerBound) ||
           (randInt > upperBound));
  return randInt;
}

template <class TSample>
void
GaussianRandomSpatialNeighborSubsampler<TSample>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Gaussian variance: " << m_Variance << std::endl;
  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
