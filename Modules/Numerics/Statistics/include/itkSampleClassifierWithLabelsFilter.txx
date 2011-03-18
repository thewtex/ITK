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
#ifndef __itkSampleClassifierWithLabelsFilter_txx
#define __itkSampleClassifierWithLabelsFilter_txx

#include "itkSampleClassifierWithLabelsFilter.h"

namespace itk
{
namespace Statistics
{

template< class TInputSample, class TLabelsSample >
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::SampleClassifierWithLabelsFilter()
{
  this->ProcessObject::SetNumberOfRequiredInputs(2);
  this->ProcessObject::SetNumberOfRequiredOutputs(1);

  this->ProcessObject::SetNthOutput( 0, this->MakeOutput(0) );
}

template< class TInputSample, class TLabelsSample >
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::~SampleClassifierWithLabelsFilter()
{
}

template< class TInputSample, class TLabelsSample >
void
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::SetLabelInput(const LabelsSampleType *sample)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 1, const_cast< LabelsSampleType * >( sample ) );
}

template< class TInputSample, class TLabelsSample >
const typename
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >::LabelsSampleType *
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::GetLabelInput() const
{
  const LabelsSampleType *input =
    static_cast< const LabelsSampleType * >( this->ProcessObject::GetInput(1) );

  return input;
}

template< class TInputSample, class TLabelsSample >
void
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}

template< class TInputSample, class TLabelsSample >
void
SampleClassifierWithLabelsFilter< TInputSample, TLabelsSample >
::GenerateData()
{
  // TODO
}

} // end of namespace Statistics
} // end of namespace itk

#endif
