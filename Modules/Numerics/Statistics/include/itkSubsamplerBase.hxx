#ifndef __itkSubsamplerBase_hxx
#define __itkSubsamplerBase_hxx
#include "itkSubsamplerBase.h"

namespace itk {
namespace Statistics {

template <class TSample>
typename SubsamplerBase<TSample>::Baseclass::Pointer
SubsamplerBase<TSample>
::Clone() const
{
  typename Baseclass::Pointer subsampler = Self::New().GetPointer()->Copy(this);
  return subsampler;
}

template <class TSample>
typename SubsamplerBase<TSample>::Baseclass*
SubsamplerBase<TSample>
::Copy(const Baseclass* other)
{
  this->SetSample(other->GetSample());
  this->m_RequestMaximumNumberOfResults = other->m_RequestMaximumNumberOfResults;
  this->m_CanSelectQuery = other->m_CanSelectQuery;
  this->SetSeed(other->m_Seed);
  return this;
}

template <class TSample>
void
SubsamplerBase<TSample>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Request maximum number of results: " << m_RequestMaximumNumberOfResults
     << std::endl;
  os << indent << "Can select query index during search: " << m_CanSelectQuery << std::endl;
  os << indent << "seed: " << m_Seed << std::endl;

  if (m_Sample)
  {
    os << indent << "Sample: " << m_Sample << std::endl;
  }
  else
  {
    os << indent << "Sample is NULL" << std::endl;
  }

  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
