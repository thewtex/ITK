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
#ifndef __itkSubsamplerBase_h
#define __itkSubsamplerBase_h

#include "itkObject.h"
#include "itkSample.h"
#include "itkSubsample.h"

namespace itk {
namespace Statistics {
/**
 * This is the base subsampler class
 *
 * TODO provide more documentation here
 */

template < class TSample >
class ITK_EXPORT SubsamplerBase : public Object
{
public:
  /** Standard class typedefs */
  typedef SubsamplerBase                        Self;
  typedef Object                                Superclass;
  typedef Self                                  Baseclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(SubsamplerBase, Object);

  /** typedef alias for the source data container */
  typedef TSample                                          SampleType;
  typedef typename SampleType::ConstPointer                SampleConstPointer;
  typedef typename TSample::MeasurementVectorType          MeasurementVectorType;
  typedef typename TSample::InstanceIdentifier             InstanceIdentifier;

  typedef Subsample<TSample>                               SubsampleType;
  typedef typename SubsampleType::Pointer                  SubsamplePointer;
  typedef typename SubsampleType::ConstIterator            SubsampleConstIterator;
  typedef typename SubsampleType::InstanceIdentifierHolder InstanceIdentifierHolder;
  typedef unsigned int                                     SeedType;

  /** Plug in the actual sample data */
  itkSetConstObjectMacro(Sample, SampleType);
  itkGetConstObjectMacro(Sample, SampleType);

  itkSetMacro(CanSelectQuery, bool);
  itkGetConstReferenceMacro(CanSelectQuery, bool);
  itkBooleanMacro(CanSelectQuery);


  itkSetMacro(Seed, SeedType);
  itkGetConstReferenceMacro(Seed, SeedType);


  /** TODO add documentation */
  virtual void RequestMaximumNumberOfResults()
  {
    if (!this->m_RequestMaximumNumberOfResults)
    {
      this->m_RequestMaximumNumberOfResults = true;
      this->Modified();
    }
  }

/** Main Search method that MUST be implemented by each subclass
 * TODO provide better documentation.*/
  virtual void Search(const InstanceIdentifier& query,
                      SubsamplePointer& results) = 0;

  /** Method to clone subsampler. */
  virtual typename Baseclass::Pointer Clone() const = 0;

protected:
  SubsamplerBase()
  {
    m_Sample = NULL;
    m_RequestMaximumNumberOfResults = true;
    m_CanSelectQuery = true;
    m_Seed =  0;
  };
  virtual ~SubsamplerBase() {};

  /** class to help with cloning */
  virtual Baseclass* Copy(const Baseclass* other);
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  SampleConstPointer m_Sample;
  bool               m_RequestMaximumNumberOfResults;
  bool               m_CanSelectQuery;
  SeedType           m_Seed;

private:
  SubsamplerBase(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

}; // end of class SubsamplerBase

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSubsamplerBase.hxx"
#endif

#endif
