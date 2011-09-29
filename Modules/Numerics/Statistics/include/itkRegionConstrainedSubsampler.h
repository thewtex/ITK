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
#ifndef __itkRegionConstrainedSubsampler_h
#define __itkRegionConstrainedSubsampler_h

#include "itkSubsamplerBase.h"

namespace itk {
namespace Statistics {
/**
 * This is an abstract subclass of SubsamplerBase that
 * constrains subsamples to be contained within a given image
 * region.
 *
 * TODO provide more documentation here
 */

template < class TSample >
class ITK_EXPORT RegionConstrainedSubsampler : public SubsamplerBase<TSample>
{
public:
  /** Standard class typedefs */
  typedef RegionConstrainedSubsampler<TSample>  Self;
  typedef SubsamplerBase<TSample>               Superclass;
  typedef typename Superclass::Baseclass        Baseclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(RegionConstrainedSubsampler, SubsamplerBase);

  /** typedef alias for the source data container */
  typedef TSample                                          SampleType;
  typedef typename SampleType::ConstPointer                SampleConstPointer;
  typedef typename TSample::MeasurementVectorType          MeasurementVectorType;
  typedef typename TSample::InstanceIdentifier             InstanceIdentifier;

  typedef Subsample<TSample>                               SubsampleType;
  typedef typename SubsampleType::Pointer                  SubsamplePointer;
  typedef typename SubsampleType::ConstIterator            SubsampleConstIterator;
  typedef typename SubsampleType::InstanceIdentifierHolder InstanceIdentifierHolder;

  /** typedefs related to image region */
  typedef typename SampleType::ImageType      ImageType;
  typedef typename SampleType::RegionType     RegionType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::SizeType        SizeType;

  /** Plug in the actual sample data */
  void SetSample(const SampleType* sample);

  /** Method to set the region constraint.
   * Any subsamples selected must ALSO be inside this region. */
  void SetRegionConstraint(const RegionType& region);

  /** Method to get the region constraint. */
  itkGetConstReferenceMacro( RegionConstraint, RegionType );

  /** Method to set UseSampleRegionForConstraint flag
   * the only way to turn this flag off is to supply a region
   * constraint via SetRegionConstraint( region ) */
  void UseSampleRegionForConstraintOn();

  /** Method to get UseSampleRegionForConstraint flag */
  itkGetConstMacro( UseSampleRegionForConstraint, bool );

/** Main Search method that MUST be implemented by each subclass
 * TODO provide better documentation.*/
  virtual void Search(const InstanceIdentifier& query,
                      SubsamplePointer& results) = 0;

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const = 0;

protected:
  RegionConstrainedSubsampler()
  {
    this->m_RequestMaximumNumberOfResults = true;
    this->m_UseSampleRegionForConstraint = true;
  };
  virtual ~RegionConstrainedSubsampler() {};

  /** class to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  bool m_UseSampleRegionForConstraint;
  RegionType m_RegionConstraint;

private:
  RegionConstrainedSubsampler(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

}; // end of class RegionConstrainedSubsampler

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegionConstrainedSubsampler.hxx"
#endif

#endif
