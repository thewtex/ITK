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
