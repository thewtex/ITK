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

#ifndef __itkLevelSetSegmentationImageBase_hxx
#define __itkLevelSetSegmentationImageBase_hxx

#include "itkLevelSetSegmentationImageBase.h"

namespace itk
{

template< class TLevelSetContainer >
LevelSetSegmentationImageBase< TLevelSetContainer >
::LevelSetSegmentationImageBase()
{
}

template< class TLevelSetContainer >
LevelSetSegmentationImageBase< TLevelSetContainer >
::~LevelSetSegmentationImageBase()
{
}


template< class TLevelSetContainer >
typename LevelSetSegmentationImageBase< TLevelSetContainer >::SegmentImageType*
LevelSetSegmentationImageBase< TLevelSetContainer >
::GetSegmentationImage( const OutputType p )
{
  // If there is a domain map
  if ( this->m_LevelSetContainer->HasDomainMap() )
    {
    this->AllocateSegmentationImageFromDomainMap();
    this->FillSegmentationBasedOnDomainMap( p );
    }
  else
    {
    // use the level-sets occupy the entire requested region of the input image
    this->AllocateSegmentationImageFromLevelSetDomain();
    this->FillSegmentationBasedOnLevelSetContainer( p );
    }
  return m_SegmentationImage;
}

template< class TLevelSetContainer >
void
LevelSetSegmentationImageBase< TLevelSetContainer >
::AllocateSegmentationImageFromDomainMap()
{
  DomainMapImageFilterPointer m_DomainMapFilter = this->m_LevelSetContainer->GetDomainMapFilter();

  m_SegmentationImage = SegmentImageType::New();
  m_SegmentationImage->SetRegions( m_DomainMapFilter->GetOutput()->GetLargestPossibleRegion() );
  m_SegmentationImage->CopyInformation( m_DomainMapFilter->GetOutput() );
  m_SegmentationImage->Allocate();
  m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );
}


template< class TLevelSetContainer >
void
LevelSetSegmentationImageBase< TLevelSetContainer >
::FillSegmentationBasedOnDomainMap( const OutputType p )
{
  InputIndexType idx;
  OutputType out;
  PointType pt;

  DomainMapImageFilterPointer m_DomainMapFilter = this->m_LevelSetContainer->GetDomainMapFilter();
  ConstDomainIteratorType map_it   = m_DomainMapFilter->GetDomainMap().begin();
  ConstDomainIteratorType map_end  = m_DomainMapFilter->GetDomainMap().end();

  while( map_it != map_end )
    {
    const IdListType* lout = map_it->second.GetIdList();
    const typename IdListImageType::RegionType *region = map_it->second.GetRegion();

    for( IdListConstIterator lIt = lout->begin(); lIt != lout->end(); ++lIt )
      {
      LevelSetPointer levelSet = this->m_LevelSetContainer->GetLevelSet( *lIt - 1 );
      SegmentImageIteratorType it( this->m_SegmentationImage, *region );
      it.GoToBegin();
      while( !it.IsAtEnd() )
        {
        idx = it.GetIndex();

        //// TODO: Will uncomment this code after other patches are passed into master branch
        //this->m_SegmentationImage->TransformIndexToPhysicalPoint( idx, pt );
        //levelSet->GetImage()->TransformPhysicalPointToIndex( pt,idx );

        if (it.Get() == 0 )
          {
          out = levelSet->Evaluate( idx );
          if (out <= p )
            {
            it.Set( *lIt );
            }
          }
        ++it;
        }
      }
    ++map_it;
    }
}


template< class TLevelSetContainer >
void
LevelSetSegmentationImageBase< TLevelSetContainer >
::FillSegmentationBasedOnLevelSetContainer( const OutputType p )
{
  InputIndexType idx;
  OutputType out;

  typename LevelSetContainerType::Iterator It = m_LevelSetContainer->Begin();
  LevelSetPointer levelSet;

  while( It != m_LevelSetContainer->End() )
    {
    levelSet = It.GetLevelSet();
    SegmentImageIteratorType it( this->m_SegmentationImage, this->m_SegmentationImage->GetLargestPossibleRegion() );
    it.GoToBegin();
    while( !it.IsAtEnd() )
      {
      idx = it.GetIndex();
      if (it.Get() == 0 )
        {
        out = levelSet->Evaluate( idx );
        if (out <= p )
          {
          it.Set( It.GetIdentifier() );
          }
        }
      ++it;
      }
    ++It;
    }
}

}

#endif // __itkLevelSetSegmentationImageBase_hxx
