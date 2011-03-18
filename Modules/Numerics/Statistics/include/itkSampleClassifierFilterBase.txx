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
#ifndef __itkSampleClassifierFilterBase_txx
#define __itkSampleClassifierFilterBase_txx

#include "itkSampleClassifierFilterBase.h"

namespace itk
{
namespace Statistics
{
template< class TInputSample >
SampleClassifierFilterBase< TInputSample >
::SampleClassifierFilterBase()
{
  this->ProcessObject::SetNumberOfRequiredInputs(1);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0) );

  this->m_NumberOfClasses = NumericTraits< IdentifierType >::Zero;
}

template< class TInputSample >
SampleClassifierFilterBase< TInputSample >
::~SampleClassifierFilterBase()
{
}

template< class TInputSample >
void
SampleClassifierFilterBase< TInputSample >
::SetInput(const InputSampleType *sample)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 0, const_cast< InputSampleType * >( sample ) );
}

template< class TInputSample >
const typename
SampleClassifierFilterBase< TInputSample >::InputSampleType *
SampleClassifierFilterBase< TInputSample >
::GetInput() const
{
  const InputSampleType *input =
    static_cast< const InputSampleType * >( this->ProcessObject::GetInput(0) );

  return input;
}

template< class TInputSample >
typename SampleClassifierFilterBase< TInputSample >::DataObjectPointer
SampleClassifierFilterBase< TInputSample >
::MakeOutput(unsigned int)
{
  return static_cast< DataObject * >( OutputType::New().GetPointer() );
}

template< class TInputSample >
const typename SampleClassifierFilterBase< TInputSample >::OutputType *
SampleClassifierFilterBase< TInputSample >
::GetOutput() const
{
  const OutputType *output =
    static_cast< const OutputType * >( this->ProcessObject::GetOutput(0) );

  return output;
}

template< class TInputSample >
void
SampleClassifierFilterBase< TInputSample >
::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Number of Classes = " << this->m_NumberOfClasses << std::endl;
}
} // end of namespace Statistics
} // end of namespace itk

#endif
