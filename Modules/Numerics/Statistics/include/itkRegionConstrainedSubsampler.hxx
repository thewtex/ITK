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

template <class TSample>
typename RegionConstrainedSubsampler<TSample>::Baseclass*
RegionConstrainedSubsampler<TSample>
::Copy(const Baseclass* other)
{
  const Self* otherPtr = dynamic_cast<const Self*>(other);
  this->Superclass::Copy(other);
  this->m_UseSampleRegionForConstraint = otherPtr->GetUseSampleRegionForConstraint();
  this->m_RegionConstraint = otherPtr->GetRegionConstraint();
  return this;
}

template <class TSample>
void
RegionConstrainedSubsampler<TSample>
::SetSample(const SampleType* sample)
{
  itkDebugMacro("setting Sample to " << sample);
  if (this->m_Sample != sample)
  {
    this->m_Sample = sample;
    if (this->m_UseSampleRegionForConstraint)
    {
      this->m_RegionConstraint = this->m_Sample->GetRegion();
    }
    this->Modified();
  }
}

template <class TSample>
void
RegionConstrainedSubsampler<TSample>
::UseSampleRegionForConstraintOn()
{
  if ( !this->m_Sample )
  {
    itkExceptionMacro(<< "Missing Input Sample");
  }

  itkDebugMacro("Using Region from Sample for region constraint "
                << this->m_Sample->GetRegion());
  if (!this->m_UseSampleRegionForConstraint)
  {
    this->m_UseSampleRegionForConstraint = true;
    this->m_RegionConstraint = this->m_Sample->GetRegion();
    this->Modified();
  }
}

template <class TSample>
void
RegionConstrainedSubsampler<TSample>
::SetRegionConstraint(const RegionType& region)
{
  itkDebugMacro("setting region constraint to " << region);
  if (this->m_RegionConstraint != region ||
      this->m_UseSampleRegionForConstraint)
  {
    this->m_RegionConstraint = region;
    this->m_UseSampleRegionForConstraint = false;
    this->Modified();
  }
}

template <class TSample>
void
RegionConstrainedSubsampler<TSample>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Use sample region for constraint: " << m_UseSampleRegionForConstraint
     << std::endl;
  os << indent << "Region constraint: " << m_RegionConstraint << std::endl;

  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
