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

#ifndef __itkLevelSetSegmentationImage_hxx
#define __itkLevelSetSegmentationImage_hxx

#include "itkLevelSetSegmentationImage.h"

namespace itk
{

template< class TIdentifier, class TImage >
void
LevelSetSegmentationImage< LevelSetContainer< TIdentifier, LevelSetDenseImage< TImage > > >
::AllocateSegmentationImageFromLevelSetDomain()
{
  typename LevelSetContainerType::Iterator It = this->m_LevelSetContainer->Begin();
  LevelSetPointer levelSet = It.GetLevelSet();
  this->m_SegmentationImage = SegmentImageType::New();
  this->m_SegmentationImage->SetRegions( levelSet->GetImage()->GetLargestPossibleRegion() );
  this->m_SegmentationImage->CopyInformation( levelSet->GetImage() );
  this->m_SegmentationImage->Allocate();
  this->m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );
}

template< class TIdentifier, class TOutput, unsigned int VDimension >
void
LevelSetSegmentationImage< LevelSetContainer< TIdentifier, WhitakerSparseLevelSetImage< TOutput, VDimension > > >
::AllocateSegmentationImageFromLevelSetDomain()
{
  typename LevelSetContainerType::Iterator It = this->m_LevelSetContainer->Begin();
  LevelSetPointer levelSet = It.GetLevelSet();
  this->m_SegmentationImage = SegmentImageType::New();
  this->m_SegmentationImage->SetRegions( levelSet->GetLabelMap()->GetLargestPossibleRegion() );
  this->m_SegmentationImage->CopyInformation( levelSet->GetLabelMap() );
  this->m_SegmentationImage->Allocate();
  this->m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );
}

template< class TIdentifier, unsigned int VDimension >
void
LevelSetSegmentationImage< LevelSetContainer< TIdentifier, ShiSparseLevelSetImage< VDimension > > >
::AllocateSegmentationImageFromLevelSetDomain()
{
  typename LevelSetContainerType::Iterator It = this->m_LevelSetContainer->Begin();
  LevelSetPointer levelSet = It.GetLevelSet();
  this->m_SegmentationImage = SegmentImageType::New();
  this->m_SegmentationImage->SetRegions( levelSet->GetLabelMap()->GetLargestPossibleRegion() );
  this->m_SegmentationImage->CopyInformation( levelSet->GetLabelMap() );
  this->m_SegmentationImage->Allocate();
  this->m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );
}


template< class TIdentifier, unsigned int VDimension >
void
LevelSetSegmentationImage< LevelSetContainer< TIdentifier, MalcolmSparseLevelSetImage< VDimension > > >
::AllocateSegmentationImageFromLevelSetDomain()
{
  typename LevelSetContainerType::Iterator It = this->m_LevelSetContainer->Begin();
  LevelSetPointer levelSet = It.GetLevelSet();
  this->m_SegmentationImage = SegmentImageType::New();
  this->m_SegmentationImage->SetRegions( levelSet->GetLabelMap()->GetLargestPossibleRegion() );
  this->m_SegmentationImage->CopyInformation( levelSet->GetLabelMap() );
  this->m_SegmentationImage->Allocate();
  this->m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );
}

}

#endif // __itkLevelSetSegmentationImage_hxx
