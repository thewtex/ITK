/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkSpatialNeighborSubsampler_h
#define itkSpatialNeighborSubsampler_h

#include "itkRegionConstrainedSubsampler.h"
#include "itkImageHelper.h"

namespace itk::Statistics
{
/**
 * \class SpatialNeighborSubsampler
 * \brief A subsampler that selects all points
 * within the specified radius of the query point.
 *
 * This class derives from RegionConstrainedSubsampler and
 * returns all points within the Radius given by
 * SetRadius(radius) as long as that point is also within
 * the RegionConstraint.  In order for this to work, a region
 * corresponding to the Sample must be provided using
 * SetSampleRegion(region)
 *
 * This class assumes that the instance identifiers in the input
 * sample correspond to the result of ComputeOffset() of the index
 * of the corresponding point in the image region.
 *
 * \sa SubsamplerBase, RegionConstrainedSubsampler
 * \sa GaussianRandomSpatialNeighborSubsampler
 * \sa UniformRandomSpatialNeighborSubsampler
 * \ingroup ITKStatistics
 */

template <typename TSample, typename TRegion>
class ITK_TEMPLATE_EXPORT SpatialNeighborSubsampler : public RegionConstrainedSubsampler<TSample, TRegion>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(SpatialNeighborSubsampler);

  /** Standard class type aliases */
  using Self = SpatialNeighborSubsampler<TSample, TRegion>;
  using Superclass = RegionConstrainedSubsampler<TSample, TRegion>;
  using Baseclass = typename Superclass::Baseclass;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(SpatialNeighborSubsampler);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** type alias alias for the source data container */
  using typename Superclass::SampleType;
  using typename Superclass::SampleConstPointer;
  using typename Superclass::MeasurementVectorType;
  using typename Superclass::InstanceIdentifier;

  using typename Superclass::SubsampleType;
  using typename Superclass::SubsamplePointer;
  using typename Superclass::SubsampleConstIterator;
  using typename Superclass::InstanceIdentifierHolder;

  /** type alias related to region */
  using typename Superclass::RegionType;
  using typename Superclass::IndexType;
  using typename Superclass::IndexValueType;
  using typename Superclass::SizeType;
  using RadiusType = typename RegionType::SizeType;

  static constexpr unsigned int ImageDimension = RegionType::ImageDimension;
  /** other helpful type alias */
  using ImageHelperType = ImageHelper<Self::ImageDimension, Self::ImageDimension>;

  /** Method to set the radius */
  /** @ITKStartGrouping */
  void
  SetRadius(const RadiusType & radius);
  void
  SetRadius(unsigned int radius);
  /** @ITKEndGrouping */
  /** Method to get the radius */
  itkGetConstReferenceMacro(Radius, RadiusType);

  /** Method to get the flag indicating that radius has been initialized */
  itkGetConstReferenceMacro(RadiusInitialized, bool);

  /** Main Search method that MUST be implemented by each subclass
   * The Search method will find all points similar to query and return
   * them as a Subsample.  The definition of similar will be subclass-
   * specific.  And could mean spatial similarity or feature similarity
   * etc.  */
  void
  Search(const InstanceIdentifier & query, SubsamplePointer & results) override;

protected:
  /**
   * Clone the current subsampler.
   * This does a complete copy of the subsampler state
   * to the new subsampler
   */
  [[nodiscard]] typename LightObject::Pointer
  InternalClone() const override;

  SpatialNeighborSubsampler();
  ~SpatialNeighborSubsampler() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  RadiusType m_Radius{};
  bool       m_RadiusInitialized{};
}; // end of class SpatialNeighborSubsampler

} // namespace itk::Statistics

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkSpatialNeighborSubsampler.hxx"
#endif

#endif
