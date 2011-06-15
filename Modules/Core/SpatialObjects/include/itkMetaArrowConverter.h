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
#ifndef __itkMetaArrowConverter_h
#define __itkMetaArrowConverter_h

#include "itkMetaConverterBase.h"
#include "itkArrowSpatialObject.h"
#include "metaArrow.h"

namespace itk
{
/** \class MetaArrowConverter
 *  \brief converts between MetaObject<->SpatialObject
 *  \sa MetaConverterBase
 *  \ingroup ITK-SpatialObjects
 */
template< unsigned int NDimensions = 3 >
class ITK_EXPORT MetaArrowConverter :
    public MetaConverterBase< NDimensions >
{
public:
  /** Standard class typedefs */
  typedef MetaArrowConverter               Self;
  typedef MetaConverterBase< NDimensions > Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetaArrowConverter, MetaConverterBase);

  typedef typename Superclass::SpatialObjectType SpatialObjectType;
  typedef typename SpatialObjectType::Pointer    SpatialObjectPointer;
  typedef typename Superclass::MetaObjectType    MetaObjectType;

  /** Specific class types for conversion */
  typedef ArrowSpatialObject<NDimensions>               ArrowSpatialObjectType;
  typedef typename ArrowSpatialObjectType::Pointer      ArrowSpatialObjectPointer;
  typedef typename ArrowSpatialObjectType::ConstPointer ArrowSpatialObjectConstPointer;
  typedef MetaArrow                                     ArrowMetaObjectType;

  /** Convert the MetaObject to Spatial Object */
  virtual SpatialObjectPointer MetaObjectToSpatialObject(const MetaObjectType *mo);

  /** Convert the SpatialObject to MetaObject */
  virtual MetaObjectType *SpatialObjectToMetaObject(const SpatialObjectType *spatialObject);

protected:
  /** Create the specific MetaObject for this class */
  virtual MetaObjectType *CreateMetaObject();

  MetaArrowConverter();
  ~MetaArrowConverter() {}

private:
  MetaArrowConverter(const Self &);   //purposely not implemented
  void operator=(const Self &);       //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkMetaArrowConverter.txx"
#endif

#endif
