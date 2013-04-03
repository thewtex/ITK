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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkLabelMapFilter_hxx
#define __itkLabelMapFilter_hxx
#include "itkLabelMapFilter.h"
#include "itkProgressReporter.h"

namespace itk
{

template< class TAssociate >
void
LabelMapFilterThreader< TAssociate >
::ThreadedExecution( const DomainType & labelObjectRange,
  const ThreadIdType threadId )
{
  typedef typename Superclass::DomainPartitionerType   DomainPartitionerType;
  typedef typename DomainPartitionerType::IteratorType IteratorType;
  size_t labelObjectCount = 0;
  for( IteratorType it = labelObjectRange.Begin(); it != labelObjectRange.End(); ++it )
    {
    ++labelObjectCount;
    }
  ProgressReporter progress( this->m_Associate, threadId, labelObjectCount );
  for( IteratorType it = labelObjectRange.Begin(); it != labelObjectRange.End(); ++it )
    {
    this->m_Associate->ThreadedProcessLabelObject( (*it).GetPointer() );
    progress.CompletedPixel();
    }
}

template< class TInputImage, class TOutputImage >
LabelMapFilter< TInputImage, TOutputImage >
::LabelMapFilter()
{
  m_Threader = ThreaderType::New();
}

template< class TInputImage, class TOutputImage >
LabelMapFilter< TInputImage, TOutputImage >
::~LabelMapFilter()
{
}

template< class TInputImage, class TOutputImage >
void
LabelMapFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  InputImagePointer input = const_cast< InputImageType * >( this->GetInput() );

  if ( !input )
        { return; }

  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}

template< class TInputImage, class TOutputImage >
void
LabelMapFilter< TInputImage, TOutputImage >
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template< class TInputImage, class TOutputImage >
void
LabelMapFilter< TInputImage, TOutputImage >
::GenerateData()
{
  this->AllocateOutputs();

  InputImageType * inputLabelMap = this->GetLabelMap();
  typename InputImageType::LabelObjectVectorType labelObjectVector = inputLabelMap->GetLabelObjects();

  typename ThreaderType::DomainType completeDomain( labelObjectVector.begin(), labelObjectVector.end() );
  this->m_Threader->Execute( this, completeDomain );
}

template< class TInputImage, class TOutputImage >
void
LabelMapFilter< TInputImage, TOutputImage >
::ThreadedProcessLabelObject( LabelObjectType *itkNotUsed(labelObject) )
{
  // do nothing
  // the subclass should override this method
}
} // end namespace itk

#endif
