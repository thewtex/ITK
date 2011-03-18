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
#ifndef __itkSampleClassifierFilterBase_h
#define __itkSampleClassifierFilterBase_h

#include "itkMembershipSample.h"
#include "itkProcessObject.h"

namespace itk
{
namespace Statistics
{
/** \class SampleClassifierFilterBase
 * \brief Base class for filters that classify Samples into MembershipSamples.
 *
 * This base class provides the basic infractructure for a filter that takes a
 * Sample as input and produces a MembershipSample as output. The user must
 * provide the number of classes in which the input sample is going to be
 * partitioned.
 *
 * \sa Sample
 * \sa MembershipSample
 */

template< class TInputSample >
class ITK_EXPORT SampleClassifierFilterBase : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef SampleClassifierFilterBase    Self;
  typedef ProcessObject                 Superclass;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;

  /** Standard macros */
  itkTypeMacro(SampleClassifierFilterBase, ProcessObject);

  /** Typedefs for Measurement vector, measurement, Instance Identifier,
   * frequency, size, size element value from the template argument TSample */
  typedef TInputSample                                    InputSampleType;
  typedef typename InputSampleType::MeasurementVectorType MeasurementVectorType;
  typedef typename InputSampleType::MeasurementType       MeasurementType;
  typedef typename InputSampleType::InstanceIdentifier    InstanceIdentifier;

  /** Output: MembershipSample types */
  typedef MembershipSample< TInputSample >    OutputType;
  typedef typename OutputType::Pointer        OutputPointer;

  /** Set/Get the number of classes in which the input sample will be partitioned. */
  itkSetMacro( NumberOfClasses, IdentifierType );
  itkGetConstReferenceMacro( NumberOfClasses, IdentifierType );

  /** Set/Get the input sample */
  virtual void SetInput(const InputSampleType *sample);
  virtual const InputSampleType * GetInput() const;

  /** Get the output subsample */
  const OutputType  * GetOutput() const;

protected:
  SampleClassifierFilterBase();
  virtual ~SampleClassifierFilterBase();
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Make a DataObject of the correct type to used as the specified output.
   * This method is automatically called when DataObject::DisconnectPipeline()
   * is called.
   *
   * \sa ProcessObject
   */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

private:
  SampleClassifierFilterBase(const Self &); //purposely not implemented
  void operator=(const Self &);          //purposely not implemented

  IdentifierType        m_NumberOfClasses;
};                                       // end of class
} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSampleClassifierFilterBase.txx"
#endif

#endif
