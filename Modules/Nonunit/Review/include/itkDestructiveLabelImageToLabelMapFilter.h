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
#ifndef __itkDestructiveLabelImageToLabelMapFilter_h
#define __itkDestructiveLabelImageToLabelMapFilter_h

#include "itkImageToImageFilter.h"
#include "itkLabelMap.h"
#include "itkLabelObject.h"

namespace itk
{
/** \class DestructiveLabelImageToLabelMapFilter
 * \brief convert a labeled image to a label collection image
 *
 * DestructiveLabelImageToLabelMapFilter converts a label image to a label map image.
 * The label in the input image are not necessarily of the same type in the input
 * and in the output image. The output label are always generated from scratch
 * for the output label objects.
 *
 * This filter is especially useful when the input label type is not an integer
 * and thus can't be used.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or
 * http://www.insight-journal.org/browse/publication/176
 *
 * \sa BinaryImageToLabelMapFilter, LabelMapToLabelImageFilter, LabelImageToLabelMapFilter
 *
 * \ingroup ImageEnhancement MathematicalMorphologyImageFilters
 * \ingroup ITK-Review
 */
template< class TInputImage,
         class TOutputImage =
            LabelMap< LabelObject< SizeValueType,
                                   ::itk::GetImageDimension< TInputImage >::ImageDimension >
                     >,
         class TFunctor = std::less< typename TInputImage::PixelType > >
class ITK_EXPORT DestructiveLabelImageToLabelMapFilter:
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef DestructiveLabelImageToLabelMapFilter           Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                           InputImageType;
  typedef TOutputImage                          OutputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef typename InputImageType::IndexType    IndexType;

  typedef typename OutputImageType::Pointer         OutputImagePointer;
  typedef typename OutputImageType::ConstPointer    OutputImageConstPointer;
  typedef typename OutputImageType::RegionType      OutputImageRegionType;
  typedef typename OutputImageType::PixelType       OutputImagePixelType;

  typedef typename OutputImageType::LabelObjectType LabelObjectType;
  typedef typename LabelObjectType::Pointer         LabelObjectPointer;
  typedef typename LabelObjectType::LengthType      LengthType;
  typedef typename LabelObjectType::LineContainerType
                                                    LineContainerType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(DestructiveLabelImageToLabelMapFilter,
               ImageToImageFilter);

  /**
   * Set/Get the value used as "background" in the input image.
   */
  itkSetMacro(InputBackgroundValue, InputImagePixelType);
  itkGetConstMacro(InputBackgroundValue, InputImagePixelType);

  /**
   * Set/Get the value used as "background" in the output image.
   * Defaults to NumericTraits<PixelType>::NonpositiveMin().
   */
  itkSetMacro(OutputBackgroundValue, OutputImagePixelType);
  itkGetConstMacro(OutputBackgroundValue, OutputImagePixelType);

protected:
  DestructiveLabelImageToLabelMapFilter();
  virtual ~DestructiveLabelImageToLabelMapFilter() {}

  void PrintSelf(std::ostream & os, Indent indent) const;

  /** DestructiveLabelImageToLabelMapFilter needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion();

  /** DestructiveLabelImageToLabelMapFilter will produce the entire output. */
  void EnlargeOutputRequestedRegion( DataObject *itkNotUsed(output) );

  virtual void BeforeThreadedGenerateData();

  virtual void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                                    ThreadIdType threadId);

  virtual void AfterThreadedGenerateData();

  /** CustomizeLabelObject can be reimplemented by the subclasses to apply a
   * customization on the label object. This is the right time to set an attribute
   * value which depends on the original label value.
   * The label is already set and shouldn't be changed in that method.
   */
  virtual void CustomizeLabelObject( const InputImagePixelType & ,
                                    LabelObjectPointer  )
  {
    // just do nothing by default
  }

private:
  DestructiveLabelImageToLabelMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &);             //purposely not implemented

  InputImagePixelType  m_InputBackgroundValue;
  OutputImagePixelType m_OutputBackgroundValue;

  typedef std::map< InputImagePixelType, LabelObjectPointer, TFunctor >
                                            LabelObjectMapType;
  typedef typename LabelObjectMapType::iterator
                                            LabelObjectMapIterator;
  typedef std::vector< LabelObjectMapType > LabelObjectVectorType;
  LabelObjectVectorType                     m_TemporaryLabelObjectMap;

}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDestructiveLabelImageToLabelMapFilter.txx"
#endif

#endif
