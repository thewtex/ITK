/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSpatialNeighborSubsampler.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSpatialNeighborSubsampler_h
#define __itkSpatialNeighborSubsampler_h

#include "itkRegionConstrainedSubsampler.h"

namespace itk {
namespace Statistics {
/**
 * Subsample consists of all points within the specified
 * radius of the query point.
 *
 * TODO provide more documentation here
 */

template < class TSample >
class ITK_EXPORT SpatialNeighborSubsampler : public RegionConstrainedSubsampler<TSample>
{
public:
  /** Standard class typedefs */
  typedef SpatialNeighborSubsampler<TSample>    Self;
  typedef RegionConstrainedSubsampler<TSample>  Superclass;
  typedef typename Superclass::Baseclass        Baseclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(SpatialNeighborSubsampler, RegionConstrainedSubsampler);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** typedef alias for the source data container */
// TODO ask ITK group how best to enforce / document that
// acceptable samples must have a corresponding ImageType?
  typedef typename Superclass::SampleType                  SampleType;
  typedef typename Superclass::SampleConstPointer          SampleConstPointer;
  typedef typename Superclass::MeasurementVectorType       MeasurementVectorType;
  typedef typename Superclass::InstanceIdentifier          InstanceIdentifier;

  typedef typename Superclass::SubsampleType            SubsampleType;
  typedef typename Superclass::SubsamplePointer         SubsamplePointer;
  typedef typename Superclass::SubsampleConstIterator   SubsampleConstIterator;
  typedef typename Superclass::InstanceIdentifierHolder InstanceIdentifierHolder;

  /** typedefs related to image region */
  typedef typename Superclass::ImageType      ImageType;
  typedef typename ImageType::SizeType        RadiusType;
  typedef typename Superclass::RegionType     RegionType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::SizeType        SizeType;


  /** Method to set the radius */
  void SetRadius(const RadiusType& radius);
  void SetRadius(unsigned int radius);

  /** Method to get the radius */
  itkGetConstReferenceMacro(Radius, RadiusType);

  /** Method to get the flag indicating that radius has been initialized */
  itkGetConstReferenceMacro(RadiusInitialized, bool);

/** Main Search method that MUST be implemented by each subclass
 * TODO provide better documentation.*/
  virtual void Search(const InstanceIdentifier& query,
                      SubsamplePointer& results);

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const;

protected:
  SpatialNeighborSubsampler() { m_RadiusInitialized = false; };
  virtual ~SpatialNeighborSubsampler() {};

  /** class to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  RadiusType m_Radius;
  bool m_RadiusInitialized;

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
