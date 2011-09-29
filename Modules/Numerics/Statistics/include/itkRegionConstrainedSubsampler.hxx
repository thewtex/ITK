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
#ifndef __itkRegionConstrainedSubsampler_hxx
#define __itkRegionConstrainedSubsampler_hxx
#include "itkRegionConstrainedSubsampler.h"

namespace itk {
namespace Statistics {

  template <class TSample, class TRegion>
typename RegionConstrainedSubsampler<TSample, TRegion>::Baseclass*
RegionConstrainedSubsampler<TSample, TRegion>
::Copy(const Baseclass* other)
{
  const Self* otherPtr = dynamic_cast<const Self*>(other);
  this->Superclass::Copy(other);
  if (otherPtr->GetRegionConstraintInitialized())
  {
    this->SetRegionConstraint(otherPtr->GetRegionConstraint());
  }
  else
  {
    this->m_RegionConstraintInitialized = false;
  }
  return this;
}

template <class TSample, class TRegion>
void
RegionConstrainedSubsampler<TSample, TRegion>
::SetRegionConstraint(const RegionType& region)
{
  itkDebugMacro("setting region constraint to " << region);
  if (this->m_RegionConstraint != region ||
      !(this->m_RegionConstraintInitialized))
  {
    this->m_RegionConstraint = region;
    this->m_RegionConstraintInitialized = true;
    this->Modified();
  }
}

template <class TSample, class TRegion>
void
RegionConstrainedSubsampler<TSample, TRegion>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  if (m_RegionConstraintInitialized)
  {
    os << indent << "Using region constraint: " << m_RegionConstraint << std::endl;
  }
  else
  {
    os << indent << "Region constraint has not been initialized!" << std::endl;
  }

  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
