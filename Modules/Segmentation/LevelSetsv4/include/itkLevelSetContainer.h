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

#include "itkLevelSetContainerBase.h"

#include "itkLevelSetDenseImageBase.h"
#include "itkWhitakerSparseLevelSetImage.h"
#include "itkShiSparseLevelSetImage.h"
#include "itkMalcolmSparseLevelSetImage.h"


namespace itk
{
template< class TIdentifier, class TLevelSet >
class LevelSetContainer
{};

template< class TIdentifier, class TImage >
class LevelSetContainer< TIdentifier, LevelSetDenseImageBase< TImage > > :
public LevelSetContainerBase< TIdentifier, LevelSetDenseImageBase< TImage > >
{
public:
  typedef LevelSetDenseImageBase< TImage >  LevelSetType;

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

  typedef typename Superclass::HeavisideType    HeavisideType;
  typedef typename Superclass::HeavisidePointer HeavisidePointer;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::IdListType               IdListType;
  typedef typename Superclass::IdListIterator           IdListIterator;
  typedef typename Superclass::IdListImageType          IdListImageType;
  typedef typename Superclass::CacheImageType           CacheImageType;
  typedef typename Superclass::DomainMapImageFilterType DomainMapImageFilterType;

  typedef typename Superclass::DomainMapImageFilterPointer  DomainMapImageFilterPointer;
  typedef typename Superclass::LevelSetDomainType           LevelSetDomainType;
  typedef typename Superclass::DomainIteratorType           DomainIteratorType;

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

protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

private:
  LevelSetContainer( const Self& );
  void operator = ( const Self& );
};

template< class TIdentifier, typename TOutput, unsigned int VDimension >
class LevelSetContainer< TIdentifier, WhitakerSparseLevelSetImage< TOutput, VDimension > > :
public LevelSetContainerBase< TIdentifier, WhitakerSparseLevelSetImage< TOutput, VDimension > >
{
public:
  typedef WhitakerSparseLevelSetImage< TOutput, VDimension >  LevelSetType;

  typedef LevelSetContainer                                   Self;
  typedef LevelSetContainerBase< TIdentifier, LevelSetType >  Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro ( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetContainer, LevelSetContainerBase );

protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

private:
  LevelSetContainer( const Self& );
  void operator = ( const Self& );
};

template< class TIdentifier, unsigned int VDimension >
class LevelSetContainer< TIdentifier, ShiSparseLevelSetImage< VDimension > > :
public LevelSetContainerBase< TIdentifier, ShiSparseLevelSetImage< VDimension > >
{
public:
  typedef ShiSparseLevelSetImage< VDimension >  LevelSetType;

  typedef LevelSetContainer                                   Self;
  typedef LevelSetContainerBase< TIdentifier, LevelSetType >  Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro ( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetContainer, LevelSetContainerBase );

protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

private:
  LevelSetContainer( const Self& );
  void operator = ( const Self& );
};

template< class TIdentifier, unsigned int VDimension >
class LevelSetContainer< TIdentifier, MalcolmSparseLevelSetImage< VDimension > > :
public LevelSetContainerBase< TIdentifier, MalcolmSparseLevelSetImage< VDimension > >
{
public:
  typedef MalcolmSparseLevelSetImage< VDimension >  LevelSetType;

  typedef LevelSetContainer                                   Self;
  typedef LevelSetContainerBase< TIdentifier, LevelSetType >  Superclass;
  typedef SmartPointer< Self >                                Pointer;
  typedef SmartPointer< const Self >                          ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro ( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetContainer, LevelSetContainerBase );

protected:
  LevelSetContainer() {}
  ~LevelSetContainer() {}

private:
  LevelSetContainer( const Self& );
  void operator = ( const Self& );
};
}

#endif // __itkLevelSetContainer_h
