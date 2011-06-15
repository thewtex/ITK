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
#ifndef __itkMetaImageMaskConverter_h
#define __itkMetaImageMaskConverter_h

//
// to avoid an Internal Compiler Error in Visual Studio 6.0
//
#include "metaImage.h"
#include "itkImageMaskSpatialObject.h"
#include "itkMetaConverterBase.h"

namespace itk
{

template< unsigned int NDimensions = 3 >
class ITK_EXPORT MetaImageMaskConverter :
    public MetaImageConverter< NDimensions, unsigned char >
{
public:
  /** Standard class typedefs */
  typedef MetaImageMaskConverter                           Self;
  typedef MetaImageConverter< NDimensions, unsigned char > Superclass;
  typedef SmartPointer< Self >                             Pointer;
  typedef SmartPointer< const Self >                       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetaImageMaskConverter, MetaImageConverter);

  typedef typename Superclass::SpatialObjectType SpatialObjectType;
  typedef typename SpatialObjectType::Pointer    SpatialObjectPointer;
  typedef typename Superclass::MetaObjectType    MetaObjectType;

  /** Specific class types for conversion */
  typedef ImageMaskSpatialObject<NDimensions> ImageMaskSpatialObjectType;
  typedef typename ImageMaskSpatialObjectType::Pointer      ImageMaskSpatialObjectPointer;
  typedef typename ImageMaskSpatialObjectType::ConstPointer ImageMaskSpatialObjectConstPointer;
  typedef MetaImage                                         ImageMaskMetaObjectType;
  typedef typename Superclass::ImageType                    ImageType;
  /** Convert the MetaObject to Spatial Object */
  virtual SpatialObjectPointer MetaObjectToSpatialObject(const MetaObjectType *mo);

  /** Convert the SpatialObject to MetaObject */
  virtual MetaObjectType *SpatialObjectToMetaObject(const SpatialObjectType *spatialObject);

protected:
  /** Create the specific MetaObject for this class */
  virtual MetaObjectType *CreateMetaObject();

  MetaImageMaskConverter();
  ~MetaImageMaskConverter() {}

private:
  MetaImageMaskConverter(const Self &);   //purposely not implemented
  void operator=(const Self &);       //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkMetaImageMaskConverter.txx"
#endif

#endif
