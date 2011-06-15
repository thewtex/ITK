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
#ifndef __itkMetaImageMaskConverter_txx
#define __itkMetaImageMaskConverter_txx

#include "itkMetaImageMaskConverter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{
/** Constructor */
template< unsigned int NDimensions>
MetaImageMaskConverter< NDimensions >
::MetaImageMaskConverter()
{}

template< unsigned int NDimensions>
typename MetaImageMaskConverter< NDimensions >::MetaObjectType *
MetaImageMaskConverter< NDimensions >
::CreateMetaObject()
{
  return dynamic_cast<MetaObjectType *>(new ImageMaskMetaObjectType);
}

/** Convert a metaImage into an ImageMaskSpatialObject  */
template< unsigned int NDimensions>
typename MetaImageMaskConverter< NDimensions >::SpatialObjectPointer
MetaImageMaskConverter< NDimensions >
::MetaObjectToSpatialObject(const MetaObjectType *mo)
{
  const ImageMaskMetaObjectType *image =
    dynamic_cast<const ImageMaskMetaObjectType *>(mo);

  ImageMaskSpatialObjectPointer imageSO = ImageMaskSpatialObjectType::New();


  typename ImageType::Pointer myImage = this->AllocateImage(image);

  itk::ImageRegionIteratorWithIndex< ImageType >
    it(myImage, myImage->GetLargestPossibleRegion());
  for ( unsigned int i = 0; !it.IsAtEnd(); i++, ++it )
    {
    it.Set(
      static_cast< typename ImageType::PixelType >( image->ElementData(i) ) );
    }

  imageSO->SetImage(myImage);
  imageSO->SetId( image->ID() );
  imageSO->SetParentId( image->ParentID() );
  imageSO->GetProperty()->SetName( image->Name() );

  return imageSO.GetPointer();
}


/** Convert an Image SpatialObject into a metaImage */
template< unsigned int NDimensions>
typename MetaImageMaskConverter< NDimensions >::MetaObjectType *
MetaImageMaskConverter< NDimensions >
::SpatialObjectToMetaObject(const SpatialObjectType *so)
{
  const ImageMaskSpatialObjectConstPointer imageSO =
    dynamic_cast<const ImageMaskSpatialObjectType *>(so);

  typedef typename ImageType::ConstPointer     ImageConstPointer;
  typedef typename ImageType::SizeType         SizeType;
  typedef typename ImageType::RegionType       RegionType;

  ImageConstPointer SOImage = imageSO->GetImage();

  float spacing[NDimensions];
  int   size[NDimensions];

  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    size[i] = SOImage->GetLargestPossibleRegion().GetSize()[i];
    spacing[i] = SOImage->GetSpacing()[i];
    }

  ImageMaskMetaObjectType *imageMaskMO =
    new MetaImage( NDimensions, size,
                   spacing, MET_GetPixelType( typeid( typename ImageType::PixelType ) ) );

  itk::ImageRegionConstIterator< ImageType > it( SOImage,
                                                 SOImage->GetLargestPossibleRegion() );
  for ( unsigned int i = 0; !it.IsAtEnd(); i++, ++it )
    {
    imageMaskMO->ElementData( i, it.Get() );
    }

  imageMaskMO->ID( imageSO->GetId() );
  if ( imageSO->GetParent() )
    {
    imageMaskMO->ParentID( imageSO->GetParent()->GetId() );
    }
  return imageMaskMO;
}

} // end namespace itk

#endif
