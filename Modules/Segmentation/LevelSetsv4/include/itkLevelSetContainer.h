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

#ifndef __itkLevelSetContainer_h
#define __itkLevelSetContainer_h

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkLevelSetContainerBase.h"

#include "itkLevelSetDenseImage.h"


namespace itk
{
/**
 *  \class LevelSetContainer
 *  \brief Container of Level-Sets
 *
 *  \tparam TIdentifier Input level set id type
 *  \tparam TLevelSet Level Set Type
 *
 *  \ingroup ITKLevelSetsv4
 */
template< class TIdentifier, class TLevelSet >
class LevelSetContainer :
public LevelSetContainerBase< TIdentifier, TLevelSet >
{
public:

  typedef LevelSetContainer                               Self;
  typedef LevelSetContainerBase< TIdentifier, TLevelSet > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro ( Self );

  itkTypeMacro ( LevelSetContainer, LevelSetContainerBase );

  typedef typename Superclass::LevelSetIdentifierType LevelSetIdentifierType;

  typedef typename Superclass::LevelSetType       LevelSetType;
  typedef typename Superclass::LevelSetPointer    LevelSetPointer;
  typedef typename Superclass::InputIndexType     InputIndexType;
  typedef typename Superclass::OutputType         OutputPixelType;
  typedef typename Superclass::OutputRealType     OutputRealType;
  typedef typename Superclass::GradientType       GradientType;
  typedef typename Superclass::HessianType        HessianType;

  typedef typename Superclass::LevelSetContainerType              LevelSetContainerType;
  typedef typename Superclass::LevelSetContainerConstIteratorType LevelSetContainerConstIteratorType;
  typedef typename Superclass::LevelSetContainerIteratorType      LevelSetContainerIteratorType;

  typedef typename Superclass::HeavisideType          HeavisideType;
  typedef typename Superclass::HeavisideConstPointer  HeavisideConstPointer;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::IdListType               IdListType;
  typedef typename Superclass::IdListIterator           IdListIterator;
  typedef typename Superclass::IdListImageType          IdListImageType;
  typedef typename Superclass::CacheImageType           CacheImageType;
  typedef typename Superclass::DomainMapImageFilterType DomainMapImageFilterType;

  typedef typename Superclass::DomainMapImageFilterPointer  DomainMapImageFilterPointer;
  typedef typename Superclass::LevelSetDomainType           LevelSetDomainType;
  typedef typename Superclass::DomainIteratorType           DomainIteratorType;

protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

private:
  LevelSetContainer( const Self& ); // purposely not implemented
  void operator = ( const Self& );  // purposely not implemented
};

/**
 *  \brief Container class for dense level sets
 *  \ingroup ITKLevelSetsv4
 */
template< class TIdentifier, class TImage >
class LevelSetContainer< TIdentifier, LevelSetDenseImage< TImage > > :
public LevelSetContainerBase< TIdentifier, LevelSetDenseImage< TImage > >
{
public:
  typedef LevelSetDenseImage< TImage >  LevelSetType;

  typedef LevelSetContainer                                   Self;
  typedef LevelSetContainerBase< TIdentifier, LevelSetType >  Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro ( Self );

  itkTypeMacro ( LevelSetContainer, LevelSetContainerBase );

  typedef typename Superclass::LevelSetIdentifierType LevelSetIdentifierType;

  typedef typename Superclass::LevelSetPointer    LevelSetPointer;
  typedef typename Superclass::InputIndexType     InputIndexType;
  typedef typename Superclass::OutputType         OutputPixelType;
  typedef typename Superclass::OutputRealType     OutputRealType;
  typedef typename Superclass::GradientType       GradientType;
  typedef typename Superclass::HessianType        HessianType;

  typedef typename Superclass::LevelSetContainerType              LevelSetContainerType;
  typedef typename Superclass::LevelSetContainerConstIteratorType LevelSetContainerConstIteratorType;
  typedef typename Superclass::LevelSetContainerIteratorType      LevelSetContainerIteratorType;

  typedef typename Superclass::HeavisideType          HeavisideType;
  typedef typename Superclass::HeavisideConstPointer  HeavisideConstPointer;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::IdListType               IdListType;
  typedef typename Superclass::IdListIterator           IdListIterator;
  typedef typename Superclass::IdListImageType          IdListImageType;
  typedef typename Superclass::CacheImageType           CacheImageType;
  typedef typename Superclass::DomainMapImageFilterType DomainMapImageFilterType;

  typedef typename Superclass::DomainMapImageFilterPointer  DomainMapImageFilterPointer;
  typedef typename Superclass::LevelSetDomainType           LevelSetDomainType;
  typedef typename Superclass::DomainIteratorType           DomainIteratorType;
  typedef typename Superclass::ConstDomainIteratorType      ConstDomainIteratorType;

  typedef Image<IdentifierType, Dimension >  SegmentImageType;
  typedef typename SegmentImageType::Pointer SegmentImagePointer;

  typedef ImageRegionIteratorWithIndex< SegmentImageType > SegmentImageIteratorType;

  typedef typename LevelSetType::ImageType    LevelSetImageType;
  typedef typename LevelSetImageType::Pointer LevelSetImagePointer;

  /** Compute information from data object and/or allocate new level set image */
  void CopyInformationAndAllocate( const Self * iOther, const bool & iAllocate )
    {
    LevelSetContainerType internalContainer = iOther->GetContainer();
    LevelSetContainerConstIteratorType it = internalContainer.begin();

    LevelSetContainerType newContainer;

    while( it != internalContainer.end() )
      {
      if( iAllocate )
        {
        LevelSetPointer temp_ls = LevelSetType::New();

        LevelSetImagePointer image = LevelSetImageType::New();
        const LevelSetImageType * otherImage = (it->second)->GetImage();

        image->CopyInformation( otherImage );
        image->SetBufferedRegion( otherImage->GetBufferedRegion() );
        image->SetRequestedRegion( otherImage->GetRequestedRegion() );
        image->SetLargestPossibleRegion( otherImage->GetLargestPossibleRegion() );
        image->Allocate();
        image->FillBuffer( NumericTraits< OutputPixelType >::Zero );

        temp_ls->SetImage( image );
        newContainer[ it->first ] = temp_ls;
        }
      else
        {
        LevelSetPointer temp_ls;
        newContainer[ it->first ] = temp_ls;
        }
      ++it;
      }

    this->SetContainer( newContainer );
    }

  /* Get a segmentation image from the level-set container */
  SegmentImageType* GetSegmentationImage( const OutputPixelType p )
    {
    m_SegmentationImage = SegmentImageType::New();
    m_SegmentationImage->SetRegions( this->Begin()->GetLevelSet()->GetImage()->GetLargestPossibleRegion() );
    m_SegmentationImage->CopyInformation( this->Begin()->GetLevelSet()->GetImage() );
    m_SegmentationImage->Allocate();
    m_SegmentationImage->FillBuffer( NumericTraits< LevelSetIdentifierType >::Zero );

    InputIndexType idx;
    OutputPixelType out;

    if ( this->HasDomainMap() )
      {// If there is a domain map
      ConstDomainIteratorType map_it   = this->GetDomainMapFilter()->GetDomainMap().begin();
      ConstDomainIteratorType map_end  = this->GetDomainMapFilter()->GetDomainMap().end();

      while( map_it != map_end )
        {
        const IdListType* lout = map_it->second.GetIdList();
        const typename IdListImageType::RegionType *region = map_it->second.GetRegion();

        for( typename IdListType::const_iterator lIt = lout->begin(); lIt != lout->end(); ++lIt )
          {
          LevelSetPointer levelSet = this->GetLevelSet( *lIt - 1 );
          SegmentImageIteratorType it( this->m_SegmentationImage, *region );
          it.GoToBegin();
          while( !it.IsAtEnd() )
            {
            idx = it.GetIndex();
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
    else // assume that the level-sets occupy the entire requested region of the input image
      {
      typename Superclass::Iterator It = this->Begin();
      while( It != this->End() )
        {
        LevelSetPointer levelSet = It.GetLevelSet();
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
    return m_SegmentationImage;
    }


protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

  SegmentImagePointer m_SegmentationImage;

private:
  LevelSetContainer( const Self& ); // purposely not implemented
  void operator = ( const Self& );  // purposely not implemented
};

}

#endif // __itkLevelSetContainer_h
