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
#ifndef __itkSampleClassifierWithLabelsFilter_h
#define __itkSampleClassifierWithLabelsFilter_h

#include "itkMembershipSample.h"
#include "itkSampleClassifierFilterBase.h"

namespace itk
{
namespace Statistics
{
/** \class SampleClassifierWithLabelsFilter
 * \brief Filter that combines a Sample with a set of labels per measurement vector.
 *
 * This filter will take two Samples as inputs and will produce a
 * MembershipSample as output. The first input Sample should contain a typical
 * set of MeasurementVectors, while the second input Sample should contain
 * measurement vectors of just a single integer component representing a label.
 *
 * The filter will produce as output a MembershipSample class that combines in
 * a single data structure a reference to the input Sample, and a labelling of
 * its measurement vectors according to the labels provided in the second input
 * to this filter.
 *
 * \sa Sample
 * \sa MembershipSample
 */

template< class TInputSample, class TLabelsSample >
class ITK_EXPORT SampleClassifierWithLabelsFilter : public SampleClassifierFilterBase< TInputSample >
{
public:
  /** Standard class typedefs. */
  typedef SampleClassifierWithLabelsFilter                Self;
  typedef SampleClassifierFilterBase< TInputSample >    Superclass;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;

  /** Standard macros */
  itkTypeMacro(SampleClassifierWithLabelsFilter, SampleClassifierFilterBase);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Typedefs for Measurement vector, measurement, Instance Identifier,
   * frequency, size, size element value from the template argument TSample */
  typedef typename Superclass::InputSampleType         InputSampleType;
  typedef typename Superclass::MeasurementVectorType   MeasurementVectorType;
  typedef typename Superclass::MeasurementType         MeasurementType;
  typedef typename Superclass::InstanceIdentifier      InstanceIdentifier;
  typedef TLabelsSample                                LabelsSampleType;

  /** Set/Get the input labels sample */
  virtual void SetLabelInput(const LabelsSampleType *sample);
  virtual const LabelsSampleType * GetLabelInput() const;

protected:
  SampleClassifierWithLabelsFilter();
  virtual ~SampleClassifierWithLabelsFilter();
  void PrintSelf(std::ostream & os, Indent indent) const;

  void GenerateData();

private:
  SampleClassifierWithLabelsFilter(const Self &); //purposely not implemented
  void operator=(const Self &);          //purposely not implemented
};                                       // end of class
} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleClassifierWithLabelsFilter.txx"
#endif

#endif
