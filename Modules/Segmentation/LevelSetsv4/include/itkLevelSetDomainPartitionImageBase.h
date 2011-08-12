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
#ifndef __itkLevelSetDomainPartitionImageBase_h
#define __itkLevelSetDomainPartitionImageBase_h

#include "itkLevelSetDomainPartitionBase.h"
#include "itkMacro.h"

#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
/** \class LevelSetDomainPartitionImageBase
 *
 * \brief Helper class used to partition domain and efficiently compute overlap.
 *
 */
template< class TImage >
class LevelSetDomainPartitionImageBase :
  public LevelSetDomainPartitionBase< TImage >
{
public:

  typedef LevelSetDomainPartitionImageBase      Self;
  typedef LevelSetDomainPartitionBase< TImage > Superclass;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  itkTypeMacro( LevelSetDomainPartitionImageBase,
                LevelSetDomainPartitionBase );

  typedef TImage                             ImageType;
  typedef typename ImageType::Pointer        ImagePointer;
  typedef typename ImageType::ConstPointer   ImageConstPointer;
  typedef typename ImageType::PixelType      PixelType;
  typedef typename ImageType::RegionType     RegionType;
  typedef typename ImageType::SizeType       SizeType;
  typedef typename SizeType::SizeValueType   SizeValueType;
  typedef typename ImageType::SpacingType    SpacingType;
  typedef typename ImageType::IndexType      IndexType;
  typedef typename IndexType::IndexValueType IndexValueType;
  typedef typename ImageType::PointType      PointType;

  typedef typename Superclass::IdentifierListType IdentifierListType;

  typedef Image< IdentifierListType, ImageDimension >   ListImageType;
  typedef typename ListImageType::Pointer               ListImagePointer;
  typedef typename ListImageType::ConstPointer          ListImageConstPointer;
  typedef typename ListImageType::RegionType            ListRegionType;
  typedef typename ListImageType::SizeType              ListSizeType;
  typedef typename ListSizeType::SizeValueType          ListSizeValueType;
  typedef typename ListImageType::SpacingType           ListSpacingType;
  typedef typename ListImageType::IndexType             ListIndexType;
  typedef typename ListIndexType::IndexValueType        ListIndexValueType;
  typedef typename ListImageType::PointType             ListPointType;
  typedef ImageRegionIteratorWithIndex< ListImageType > ListIteratorType;

  void SetImage( ImagePointer iImage )
  {
    m_Image = iImage;
  }

protected:
  LevelSetDomainPartitionImageBase() : Superclass()
  {}

  virtual ~LevelSetDomainPartitionImageBase(){}

  ImagePointer     m_Image;
  ListImagePointer m_ListDomain;

  /** Populate a list image with each pixel being a list of overlapping
   *  level set support at that pixel */
  virtual void PopulateListDomain()
  {
    ListSpacingType spacing = this->m_ListDomain->GetSpacing();

    ListRegionType region = this->m_ListDomain->GetLargestPossibleRegion();

    ListIteratorType lIt(this->m_ListDomain, region);

    for ( lIt.GoToBegin(); !lIt.IsAtEnd(); ++lIt )
      {
      ListIndexType ind = lIt.GetIndex();
      IdentifierListType L;
      for ( unsigned int i = 0; i < this->m_FunctionCount; i++ )
        {
        if ( this->m_LevelSetDataPointerVector[i]->VerifyInsideRegion(ind) )
          {
          L.push_back(i);
          }
        }
      lIt.Set(L);
      }
  }
  /** Allocate a list image with each pixel being a list of overlapping
   *  level set support at that pixel */
  void AllocateListDomain()
  {
    if( m_Image.IsNull() )
      {
      itkGenericExceptionMacro( "m_Image is null" );
      }
    this->m_ListDomain = ListImageType::New();
    this->m_ListDomain->CopyInformation( m_Image );
    this->m_ListDomain->SetRegions( m_Image->GetLargestPossibleRegion() );
    this->m_ListDomain->Allocate();
  }

private:
  LevelSetDomainPartitionImageBase(const Self &); //purposely not
                                                       // implemented
  void operator=(const Self &);                        //purposely not
                                                       // implemented
};
} //end namespace itk

#endif
