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
#ifndef __itkAttributeOpeningLabelMapFilter_h
#define __itkAttributeOpeningLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"
#include "itkAttributeLabelObject.h"

namespace itk {
/** \class AttributeOpeningLabelMapFilter
 * \brief remove the objects according to the value of their attribute
 *
 * AttributeOpeningLabelMapFilter removes the objects in a label collection image
 * with an attribute value smaller or greater than a threshold called Lambda.
 * The attribute is provide by an attribute accessor given in template parameter.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or
 * http://www.insight-journal.org/browse/publication/176
 *
 * \sa AttributeLabelObject
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 * \ingroup ITKLabelMap
 */
template<typename TImage, typename TAttributeAccessor=
           typename Functor::AttributeLabelObjectAccessor< typename TImage::LabelObjectType > >
class AttributeOpeningLabelMapFilter :
  public InPlaceLabelMapFilter<TImage>
{
public:
  /** Standard class typedefs. */
  typedef AttributeOpeningLabelMapFilter Self;
  typedef InPlaceLabelMapFilter<TImage>  Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  /** Some convenient typedefs. */
  typedef TImage                           ImageType;
  typedef typename ImageType::Pointer      ImagePointer;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::PixelType    PixelType;
  typedef typename ImageType::IndexType    IndexType;

  typedef typename Superclass::LabelObjectType LabelObjectType;

  typedef TAttributeAccessor                                 AttributeAccessorType;
  typedef typename AttributeAccessorType::AttributeValueType AttributeValueType;

  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(AttributeOpeningLabelMapFilter,
               InPlaceLabelMapFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
/*  itkConceptMacro(InputEqualityComparableCheck,
    (Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(IntConvertibleToInputCheck,
    (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(InputOStreamWritableCheck,
    (Concept::OStreamWritable<InputImagePixelType>));*/
// End concept checking
#endif

  /**
   * Set/Get the threshold used to keep or remove the objects.
   */
  itkGetConstMacro(Lambda, AttributeValueType);
  itkSetMacro(Lambda, AttributeValueType);

  /**
   * Set/Get the ordering of the objects. By default, the objects with
   * an attribute value smaller than Lamba are removed. Turning ReverseOrdering
   * to true make this filter remove the object with an attribute value greater
   * than Lambda instead.
   */
  itkGetConstMacro( ReverseOrdering, bool );
  itkSetMacro( ReverseOrdering, bool );
  itkBooleanMacro( ReverseOrdering );

protected:
  AttributeOpeningLabelMapFilter();
  ~AttributeOpeningLabelMapFilter() {
  }

  void GenerateData();

  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  AttributeOpeningLabelMapFilter(const Self&); //purposely not implemented
  void operator=(const Self&);                 //purposely not implemented

  AttributeValueType m_Lambda;
  bool               m_ReverseOrdering;

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAttributeOpeningLabelMapFilter.hxx"
#endif

#endif
