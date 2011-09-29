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
#ifndef __itkLevelSetContainerSource_hxx
#define __itkLevelSetContainerSource_hxx

#include "itkLevelSetContainerSource.h"

namespace itk
{
template< class TOutputLevelSetContainer >
LevelSetContainerSource< TOutputLevelSetContainer >
::LevelSetContainerSource()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type TOutputLevelSetContainer
  OutputLevelSetContainerPointer output =
    static_cast< TOutputLevelSetContainer * >( this->MakeOutput(0).GetPointer() );

  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput( 0, output.GetPointer() );
}

template< class TOutputLevelSetContainer >
typename LevelSetContainerSource< TOutputLevelSetContainer >::DataObjectPointer
LevelSetContainerSource< TOutputLevelSetContainer >
::MakeOutput(unsigned int)
{
  return static_cast< DataObject * >( TOutputLevelSetContainer::New().GetPointer() );
}

template< class TOutputLevelSetContainer >
typename LevelSetContainerSource< TOutputLevelSetContainer >::OutputLevelSetContainerType *
LevelSetContainerSource< TOutputLevelSetContainer >
::GetOutput(void)
{
  return static_cast< TOutputLevelSetContainer * >( this->GetPrimaryOutput() );
}

template< class TOutputLevelSetContainer >
typename LevelSetContainerSource< TOutputLevelSetContainer >::OutputLevelSetContainerType *
LevelSetContainerSource< TOutputLevelSetContainer >
::GetOutput(unsigned int idx)
{
  return static_cast< TOutputLevelSetContainer * >( this->ProcessObject::GetOutput(idx) );
}

template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::SetOutput(OutputLevelSetContainerType *output)
{
  itkWarningMacro(
    <<
    "SetOutput(): This method is slated to be removed from ITK.  Please use GraftOutput() in possible combination with DisconnectPipeline() instead.");
  this->ProcessObject::SetNthOutput(0, output);
}

/**
 *
 */
template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
}

template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::GraftOutput(DataObject *graft)
{
  this->GraftNthOutput(0, graft);
}

template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::GraftOutput(const DataObjectIdentifierType & key, DataObject *graft)
{
  if ( !graft )
    {
    itkExceptionMacro(<< "Requested to graft output that is a NULL pointer");
    }

  // we use the process object method since all out output may not be
  // of the same type
  DataObject *output = this->ProcessObject::GetOutput(key);

  // Call GraftImage to copy meta-information, regions, and the pixel container
  output->Graft(graft);
}

template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::GraftNthOutput(unsigned int idx, DataObject *graft)
{
  if ( idx >= this->GetNumberOfIndexedOutputs() )
    {
    itkExceptionMacro(<< "Requested to graft output " << idx
                      << " but this filter only has " << this->GetNumberOfIndexedOutputs()
                      << " indexed Outputs.");
    }
  this->GraftOutput( this->MakeNameFromIndex(idx), graft );
}

template< class TOutputLevelSetContainer >
void
LevelSetContainerSource< TOutputLevelSetContainer >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk

#endif
