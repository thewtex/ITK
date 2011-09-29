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
#ifndef __itkLevelSetContainerToLevelSetContainerFilter_hxx
#define __itkLevelSetContainerToLevelSetContainerFilter_hxx

#include "itkLevelSetContainerToLevelSetContainerFilter.h"

namespace itk
{
/**
 *
 */
template< class TInputLevelSetContainer, class TOutputLevelSetContainer >
LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >
::LevelSetContainerToLevelSetContainerFilter()
{
  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(1);
}

/**
 *
 */
template< class TInputLevelSetContainer, class TOutputLevelSetContainer >
void
LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >
::SetInput(const TInputLevelSetContainer *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 0,
                                    const_cast< TInputLevelSetContainer * >( input ) );
}

/**
 *
 */
template< class TInputLevelSetContainer, class TOutputLevelSetContainer >
const typename LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >::InputLevelSetContainerType *
LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >
::GetInput() const
{
  return static_cast< const TInputLevelSetContainer * >( this->GetPrimaryInput() );
}

/**
 *
 */
template< class TInputLevelSetContainer, class TOutputLevelSetContainer >
const typename LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >::InputLevelSetContainerType *
LevelSetContainerToLevelSetContainerFilter< TInputLevelSetContainer, TOutputLevelSetContainer >
::GetInput(unsigned int idx) const
{
  return dynamic_cast< const TInputLevelSetContainer * >
         ( this->ProcessObject::GetInput(idx) );
}


}


} // end namespace itk

#endif
