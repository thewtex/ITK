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
#ifndef __itkSpatialNeighborSubsampler_h
#define __itkSpatialNeighborSubsampler_h

#include "itkRegionConstrainedSubsampler.h"
#include "itkImageHelper.h"

namespace itk {
namespace Statistics {
/** \class SpatialNeighborSubsampler
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

template < class TSample, class TRegion >
  class ITK_EXPORT SpatialNeighborSubsampler : public RegionConstrainedSubsampler<TSample, TRegion>
{
public:
  /** Standard class typedefs */
  typedef SpatialNeighborSubsampler<TSample, TRegion>    Self;
  typedef RegionConstrainedSubsampler<TSample, TRegion>  Superclass;
  typedef typename Superclass::Baseclass                 Baseclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(SpatialNeighborSubsampler, RegionConstrainedSubsampler);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** typedef alias for the source data container */
  typedef typename Superclass::SampleType                  SampleType;
  typedef typename Superclass::SampleConstPointer          SampleConstPointer;
  typedef typename Superclass::MeasurementVectorType       MeasurementVectorType;
  typedef typename Superclass::InstanceIdentifier          InstanceIdentifier;

  typedef typename Superclass::SubsampleType            SubsampleType;
  typedef typename Superclass::SubsamplePointer         SubsamplePointer;
  typedef typename Superclass::SubsampleConstIterator   SubsampleConstIterator;
  typedef typename Superclass::InstanceIdentifierHolder InstanceIdentifierHolder;

  /** typedefs related to region */
  typedef typename Superclass::RegionType     RegionType;
  typedef typename Superclass::IndexType      IndexType;
  typedef typename Superclass::SizeType       SizeType;
  typedef typename RegionType::SizeType       RadiusType;

  itkStaticConstMacro(ImageDimension, unsigned int, RegionType::ImageDimension);
  /** other helpful typedefs */
  typedef ImageHelper<itkGetStaticConstMacro(ImageDimension),
                      itkGetStaticConstMacro(ImageDimension)> ImageHelperType;

  /** Method to set the sample domain.
   * This should correspond to the entire region of the input sample. */
  void SetSampleRegion(const RegionType& region);

  /** Method to get the sample domain. */
  itkGetConstReferenceMacro( SampleRegion, RegionType );

  /** Method to get the flag indicating that the sample region has been initialized */
  itkGetConstReferenceMacro(SampleRegionInitialized, bool);

  /** Method to set the radius */
  void SetRadius(const RadiusType& radius);
  void SetRadius(unsigned int radius);

  /** Method to get the radius */
  itkGetConstReferenceMacro(Radius, RadiusType);

  /** Method to get the flag indicating that radius has been initialized */
  itkGetConstReferenceMacro(RadiusInitialized, bool);

  /** Main Search method that MUST be implemented by each subclass
   * The Search method will find all points similar to query and return
   * them as a Subsample.  The definition of similar will be subclass-
   * specific.  And could mean spatial similarity or feature similarity
   * etc.  */
  virtual void Search(const InstanceIdentifier& query,
                      SubsamplePointer& results);

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const;

protected:
  SpatialNeighborSubsampler() {
    m_RadiusInitialized = false;
    m_SampleRegionInitialized = false;
  };
  virtual ~SpatialNeighborSubsampler() {};

  /** class to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  RadiusType m_Radius;
  bool       m_RadiusInitialized;
  RegionType m_SampleRegion;
  bool       m_SampleRegionInitialized;

private:
  SpatialNeighborSubsampler(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

}; // end of class SpatialNeighborSubsampler

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpatialNeighborSubsampler.hxx"
#endif

#endif
