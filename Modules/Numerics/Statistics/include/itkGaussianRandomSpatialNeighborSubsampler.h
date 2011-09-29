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
#ifndef __itkGaussianRandomSpatialNeighborSubsampler_h
#define __itkGaussianRandomSpatialNeighborSubsampler_h

#include "itkUniformRandomSpatialNeighborSubsampler.h"

namespace itk {
namespace Statistics {
/**
 * Subsample consists of all points within the specified
 * shaped neighborhood of the query point.
 *
 * TODO provide more documentation here
 */

template < class TSample >
class ITK_EXPORT GaussianRandomSpatialNeighborSubsampler : public UniformRandomSpatialNeighborSubsampler<TSample>
{
public:
  /** Standard class typedefs */
  typedef GaussianRandomSpatialNeighborSubsampler<TSample>  Self;
  typedef UniformRandomSpatialNeighborSubsampler<TSample>   Superclass;
  typedef typename Superclass::Baseclass                    Baseclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(GaussianRandomSpatialNeighborSubsampler,
               UniformRandomSpatialNeighborSubsampler);

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

  typedef typename Superclass::SearchSizeType SearchSizeType;
  typedef typename Superclass::RandomIntType  RandomIntType;
  /** typedefs related to image region */
  typedef typename Superclass::ImageType      ImageType;
  typedef typename Superclass::RadiusType     RadiusType;
  typedef typename Superclass::RegionType     RegionType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::SizeType        SizeType;

  typedef double RealType;

  typedef typename Superclass::RandomGeneratorType RandomGeneratorType;
  /** Default sampling variance */
  itkStaticConstMacro(DefaultVariance, RealType, 900);

  /** Set the variance */
  itkSetMacro(Variance, RealType);

  /** Get the variance */
  itkGetConstMacro(Variance, RealType);

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const;

protected:
  GaussianRandomSpatialNeighborSubsampler()
  {
    m_Variance = DefaultVariance;
  };
  virtual ~GaussianRandomSpatialNeighborSubsampler() {};

  /** class to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

/** method to randomly generate an integer in the closed range
   * [0, upperBound]
   * usign a gaussian selection method. */
  virtual RandomIntType GetIntegerVariate(RandomIntType lowerBound,
                                          RandomIntType upperBound,
                                          RandomIntType mean);

  RealType m_Variance;

private:
  GaussianRandomSpatialNeighborSubsampler(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

}; // end of class GaussianRandomSpatialNeighborSubsampler

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGaussianRandomSpatialNeighborSubsampler.hxx"
#endif

#endif
