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
#ifndef __itkMeshSource_hpp
#define __itkMeshSource_hpp

#include "itkMeshSource.h"

namespace itk
{
/**
 *
 */
template< class TOutputMesh >
MeshSource< TOutputMesh >
::MeshSource()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type TOutputMesh
  OutputMeshPointer output =
    static_cast< TOutputMesh * >( this->MakeOutput(0).GetPointer() );

  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput( 0, output.GetPointer() );

  m_GenerateDataRegion = 0;
  m_GenerateDataNumberOfRegions = 0;
}

/**
 *
 */
template< class TOutputMesh >
typename MeshSource< TOutputMesh >::DataObjectPointer
MeshSource< TOutputMesh >
::MakeOutput(unsigned int)
{
  return static_cast< DataObject * >( TOutputMesh::New().GetPointer() );
}

/**
 *
 */
template< class TOutputMesh >
typename MeshSource< TOutputMesh >::OutputMeshType *
MeshSource< TOutputMesh >
::GetOutput(void)
{
  if ( this->GetNumberOfOutputs() < 1 )
    {
    return 0;
    }

  return static_cast< TOutputMesh * >
         ( this->ProcessObject::GetOutput(0) );
}

/**
 *
 */
template< class TOutputMesh >
typename MeshSource< TOutputMesh >::OutputMeshType *
MeshSource< TOutputMesh >
::GetOutput(unsigned int idx)
{
  return static_cast< TOutputMesh * >
         ( this->ProcessObject::GetOutput(idx) );
}

/**
 *
 */
template< class TOutputMesh >
void
MeshSource< TOutputMesh >
::SetOutput(OutputMeshType *output)
{
  itkWarningMacro(
    <<
    "SetOutput(): This method is slated to be removed from ITK.  Please use GraftOutput() in possible combination with DisconnectPipeline() instead.");
  this->ProcessObject::SetNthOutput(0, output);
}

/**
 *
 */
template< class TOutputMesh >
void
MeshSource< TOutputMesh >
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
}

/**
 *
 */
template< class TOutputMesh >
void
MeshSource< TOutputMesh >
::GraftOutput(DataObject *graft)
{
  this->GraftNthOutput(0, graft);
}

/**
 *
 */
template< class TOutputMesh >
void
MeshSource< TOutputMesh >
::GraftNthOutput(unsigned int idx, DataObject *graft)
{
  if ( idx >= this->GetNumberOfOutputs() )
    {
    itkExceptionMacro(<< "Requested to graft output " << idx
                      << " but this filter only has " << this->GetNumberOfOutputs() << " Outputs.");
    }

  if ( !graft )
    {
    itkExceptionMacro(<< "Requested to graft output that is a NULL pointer");
    }

  DataObject *output = this->GetOutput(idx);

  // Call Graft on the Mesh in order to copy meta-information, and containers.
  output->Graft(graft);
}

/**
 *
 */
template< class TOutputMesh >
void
MeshSource< TOutputMesh >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk

#endif
