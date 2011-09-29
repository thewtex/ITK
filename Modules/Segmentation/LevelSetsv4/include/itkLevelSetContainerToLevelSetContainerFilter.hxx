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
template< class TLevelSetContainer >
LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >
::LevelSetContainerToLevelSetContainerFilterBase()
{
  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(1);
}

/**
 *
 */
template< class TLevelSetContainer >
void
LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >
::SetInput(const LevelSetContainerType *input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput( 0,
                                    const_cast< LevelSetContainerType * >( input ) );
}

/**
 *
 */
template< class TLevelSetContainer >
const typename LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >::LevelSetContainerType *
LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >
::GetInput() const
{
  return static_cast< const LevelSetContainerType * >( this->GetPrimaryInput() );
}

/**
 *
 */
template< class TLevelSetContainer >
const typename LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >::LevelSetContainerType *
LevelSetContainerToLevelSetContainerFilterBase< TLevelSetContainer >
::GetInput(unsigned int idx) const
{
  return dynamic_cast< const LevelSetContainerType * >
         ( this->ProcessObject::GetInput(idx) );
}


//template< class TIdentifier, class TLevelSet >
//void
//LevelSetContainerToLevelSetContainerFilter<
//  TIdentifier,
//  DenseLevelSetContainer< TIdentifier, TLevelSet > >::
//CopyInputToOutput()
//{
//  const LevelSetContainerType* input = this->GetInput();
//  LevelSetContainerType* output = this->GetOutput();

//  typename LevelSetContainerType::ConstIterator it = input->Begin();

//  typedef ImageDuplicator< LevelSetImageType > ImageDuplicatorType;

//  while( it != input->End )
//    {
//    LevelSetType* ls = it->GetLevelSet();
//    const LevelSetImageType* lsImage = ls->GetImage();

//    LevelSetIdentifierType id = it->GetIdentifier();

//    LevelSetPointer outputLS = LevelSetType::New();

//    typename ImageDuplicatorType::Pointer duplicator = ImageDuplicatorType::New();
//    duplicator->SetInputImage( lsImage );
//    duplicator->Update();

//    outputLS->SetImage( duplicator->GetOutput() );

//    output->AddLevelSet( id, outputLS, true );
//    ++it;
//    }
//}


} // end namespace itk

#endif
