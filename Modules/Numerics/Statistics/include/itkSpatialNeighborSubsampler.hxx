#ifndef __itkSpatialNeighborSubsampler_hxx
#define __itkSpatialNeighborSubsampler_hxx
#include "itkSpatialNeighborSubsampler.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk {
namespace Statistics {

template <class TSample>
typename SpatialNeighborSubsampler<TSample>::Baseclass::Pointer
SpatialNeighborSubsampler<TSample>
::Clone() const
{
  typename Baseclass::Pointer subsampler = Self::New().GetPointer()->Copy(this);
  return subsampler;
}

template <class TSample>
typename SpatialNeighborSubsampler<TSample>::Baseclass*
SpatialNeighborSubsampler<TSample>
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
  return this;
}

template <class TSample>
void
SpatialNeighborSubsampler<TSample>
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

template <class TSample>
void
SpatialNeighborSubsampler<TSample>
::SetRadius(unsigned int radius)
{
  RadiusType radiusND;
  radiusND.Fill(radius);
  this->SetRadius(radiusND);
}

template <class TSample>
void
SpatialNeighborSubsampler<TSample>
::Search(const InstanceIdentifier& query,
         SubsamplePointer& results)
{
  if (!m_RadiusInitialized)
  {
    itkExceptionMacro(<< "Radius not set.");
  }
  results->Clear();
  results->SetSample(this->m_Sample);

  RegionType searchRegion;
  IndexType searchIndex;
  SizeType searchSize;

  IndexType constraintIndex = this->m_RegionConstraint.GetIndex();
  SizeType constraintSize = this->m_RegionConstraint.GetSize();

  typename SampleType::NeighborhoodIteratorType queryNeighborhood;
  IndexType queryIndex;

  queryNeighborhood = this->m_Sample->GetMeasurementVector(query)[0];
  queryIndex = queryNeighborhood.GetIndex(queryNeighborhood.GetCenterNeighborhoodIndex());

  for (unsigned int dim = 0; dim < ImageType::ImageDimension; ++dim)
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

  typename ImageType::ConstPointer image = this->m_Sample->GetImage();
  itk::ImageRegionConstIteratorWithIndex<ImageType> it(image,
                                                       searchRegion);

  // TODO find a cleaner way to optimize whether we need to check for
  // selecting query index than just duplicating the loops
  if (this->m_CanSelectQuery) {
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      results->AddInstance(image->ComputeOffset(it.GetIndex()));
    }
  }
  else
  {
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      if (it.GetIndex() != queryIndex)
      {
        results->AddInstance(image->ComputeOffset(it.GetIndex()));
      }
    }
  }
} // end Search method

template <class TSample>
void
SpatialNeighborSubsampler<TSample>
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
  os << std::endl;
}

}// end namespace Statistics
}// end namespace itk

#endif
