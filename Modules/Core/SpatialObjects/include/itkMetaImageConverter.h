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
#ifndef __itkMetaImageConverter_h
#define __itkMetaImageConverter_h

// to avoid an Internal Compiler Error in Visual Studio 6.0
#include "metaImage.h"
#include "itkImageMaskSpatialObject.h"

namespace itk
{
/** \class MetaImageConverter
 * \brief This is the MetaImageConverter class.
 *
 * \ingroup ITK-SpatialObjects
 */
template< unsigned int NDimensions = 3, class PixelType = unsigned char >
class ITK_EXPORT MetaImageConverter
{
public:

  MetaImageConverter();
  ~MetaImageConverter() {}

  typedef itk::ImageSpatialObject< NDimensions, PixelType > SpatialObjectType;
  typedef typename SpatialObjectType::TransformType         TransformType;
  typedef itk::ImageMaskSpatialObject< NDimensions >        MaskSpatialObjectType;

  typedef typename SpatialObjectType::Pointer     SpatialObjectPointer;
  typedef typename MaskSpatialObjectType::Pointer MaskSpatialObjectPointer;

  SpatialObjectPointer ReadMeta(const char *name);

  bool WriteMeta(SpatialObjectType *spatialObject, const char *name);

  SpatialObjectPointer MetaImageToImageSpatialObject(MetaImage *image);

  MaskSpatialObjectPointer MetaImageToImageMaskSpatialObject(MetaImage *image);

  MetaImage * ImageSpatialObjectToMetaImage(SpatialObjectType *spatialObject);
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkMetaImageConverter.txx"
#endif

#endif
