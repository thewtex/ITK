/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkUniformRandomSpatialNeighborSubsampler.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkUniformRandomSpatialNeighborSubsampler_h
#define __itkUniformRandomSpatialNeighborSubsampler_h

#include "itkSpatialNeighborSubsampler.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

namespace itk {
namespace Statistics {
/**
 * Subsample consists of all points within the specified
 * shaped neighborhood of the query point.
 *
 * TODO provide more documentation here
 */

template < class TSample >
class ITK_EXPORT UniformRandomSpatialNeighborSubsampler : public SpatialNeighborSubsampler<TSample>
{
public:
  /** Standard class typedefs */
  typedef UniformRandomSpatialNeighborSubsampler<TSample>       Self;
  typedef SpatialNeighborSubsampler<TSample>                    Superclass;
  typedef typename Superclass::Baseclass                        Baseclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(UniformRandomSpatialNeighborSubsampler, SpatialNeighborSubsampler);

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
  typedef typename Baseclass::SeedType                  SeedType;

  typedef unsigned long                       SearchSizeType;
  typedef unsigned int                        RandomIntType;

  /** typedefs related to image region */
  typedef typename Superclass::ImageType      ImageType;
  typedef typename Superclass::RadiusType     RadiusType;
  typedef typename Superclass::RegionType     RegionType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::SizeType        SizeType;

  /** typedefs related to random variate generator */
  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator RandomGeneratorType;

  virtual void SetSeed(const SeedType seed)
  {
    Superclass::SetSeed(seed);
    this->m_RandomNumberGenerator->SetSeed(this->m_Seed);
  }

  virtual void SetUseClockForSeed(const bool& useClock)
  {
    if (useClock != this->m_UseClockForSeed)
    {
      this->m_UseClockForSeed = useClock;
      if (this->m_UseClockForSeed)
      {
        this->m_RandomNumberGenerator->SetSeed();
      }
      this->Modified();
    }
  }

  itkBooleanMacro(UseClockForSeed);
  itkGetConstMacro(UseClockForSeed, bool);

  virtual void SetNumberOfResultsRequested(const SearchSizeType& numberRequested)
  {
    itkDebugMacro("setting NumberOfResultsRequested to " << numberRequested);
    if (this->m_NumberOfResultsRequested != numberRequested ||
        this->m_RequestMaximumNumberOfResults)
    {
      this->m_NumberOfResultsRequested = numberRequested;
      this->m_RequestMaximumNumberOfResults = false;
      this->Modified();
    }
  }

/** Main Search method that MUST be implemented by each subclass
 * TODO provide better documentation.*/
  virtual void Search(const InstanceIdentifier& query,
                      SubsamplePointer& results);

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const;

protected:
  UniformRandomSpatialNeighborSubsampler()
  {
    m_RandomNumberGenerator = RandomGeneratorType::New();
    m_UseClockForSeed = false;
    m_RandomNumberGenerator->SetSeed(this->m_Seed);
  };
  virtual ~UniformRandomSpatialNeighborSubsampler() {};

  /** method to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** method to randomly generate an integer in the closed range
   * [lowerBound, upperBound]
   * using a uniform sampling selection method.
   * override this method to do gaussian selection */
  virtual RandomIntType GetIntegerVariate(RandomIntType lowerBound,
                                          RandomIntType upperBound,
                                          RandomIntType itkNotUsed(mean));

  SearchSizeType m_NumberOfResultsRequested;
  RandomGeneratorType::Pointer m_RandomNumberGenerator;
  bool m_UseClockForSeed;

private:
  UniformRandomSpatialNeighborSubsampler(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

}; // end of class UniformRandomSpatialNeighborSubsampler

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkUniformRandomSpatialNeighborSubsampler.hxx"
#endif

#endif
