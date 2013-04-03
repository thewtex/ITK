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
#ifndef __itkLabelMapToBinaryImageFilter_h
#define __itkLabelMapToBinaryImageFilter_h

#include "itkLabelMapFilter.h"
#include "itkDomainThreader.h"
#include "itkThreadedImageRegionPartitioner.h"

namespace itk
{

/** \class LabelMapToBinaryImageFilterThreader
 *
 * \brief DomainThreader class to operate on the output image.
 */
template< class TAssociate >
class LabelMapToBinaryImageFilterThreader:
  public DomainThreader< ThreadedImageRegionPartitioner< TAssociate::OutputImageDimension >, TAssociate >
{
public:
  /** Standard class typedefs */
  typedef LabelMapToBinaryImageFilterThreader                                                              Self;
  typedef DomainThreader< ThreadedImageRegionPartitioner< TAssociate::OutputImageDimension >, TAssociate > Superclass;
  typedef SmartPointer< Self >                                                                             Pointer;

  /** Some convenient typedefs. */
  typedef typename Superclass::DomainType DomainType;
  typedef TAssociate                      AssociateType;

  typedef typename AssociateType::OutputImageType OutputImageType;

  /** Standard New method. */
  itkNewMacro( Self );

protected:
  LabelMapToBinaryImageFilterThreader() {}

  virtual void ThreadedExecution( const DomainType & subDomain,
    const ThreadIdType threadId );

private:
  LabelMapToBinaryImageFilterThreader( const Self & ); // purposely not implemented
  void operator=( const Self & );                      // purposely not implemented
};

/** \class LabelMapToBinaryImageFilter
 * \brief Convert a LabelMap to a binary image.
 *
 * LabelMapToBinaryImageFilter to a binary image. All the objects in the image
 * are used as foreground.  The background values of the original binary image
 * can be restored by passing this image to the filter with the
 * SetBackgroundImage() method.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or
 * http://www.insight-journal.org/browse/publication/176
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction,
 * INRA de Jouy-en-Josas, France.
 *
 * \sa LabelMapToLabelImageFilter, LabelMapMaskImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 * \ingroup ITKLabelMap
 */
template< class TInputImage, class TOutputImage >
class ITK_EXPORT LabelMapToBinaryImageFilter:
  public LabelMapFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef LabelMapToBinaryImageFilter                 Self;
  typedef LabelMapFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                              InputImageType;
  typedef TOutputImage                             OutputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename InputImageType::LabelObjectType LabelObjectType;

  typedef typename OutputImageType::Pointer      OutputImagePointer;
  typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
  typedef typename OutputImageType::RegionType   OutputImageRegionType;
  typedef typename OutputImageType::PixelType    OutputImagePixelType;
  typedef typename OutputImageType::IndexType    IndexType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(LabelMapToBinaryImageFilter, ImageToImageFilter);

  /**
   * Set/Get the value used as "background" in the output image.
   * Defaults to NumericTraits<PixelType>::NonpositiveMin().
   */
  itkSetMacro(BackgroundValue, OutputImagePixelType);
  itkGetConstMacro(BackgroundValue, OutputImagePixelType);

  /**
   * Set/Get the value used as "foreground" in the output image.
   * Defaults to NumericTraits<PixelType>::max().
   */
  itkSetMacro(ForegroundValue, OutputImagePixelType);
  itkGetConstMacro(ForegroundValue, OutputImagePixelType);

  /** Set/Get the background image top be used to restore the background values
    */
  void SetBackgroundImage(const OutputImageType *input)
  {
    // Process object is not const-correct so the const casting is required.
    this->SetNthInput( 1, const_cast< OutputImageType * >( input ) );
  }

  OutputImageType * GetBackgroundImage()
  {
    return static_cast< OutputImageType * >( const_cast< DataObject * >( this->ProcessObject::GetInput(1) ) );
  }

  /** Set the input image */
  void SetInput1(const InputImageType *input)
  {
    this->SetInput(input);
  }

  /** Set the marker image */
  void SetInput2(const OutputImageType *input)
  {
    this->SetBackgroundImage(input);
  }

protected:
  LabelMapToBinaryImageFilter();
  ~LabelMapToBinaryImageFilter() {}

  /** LabelMapToBinaryImageFilter needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  virtual void GenerateInputRequestedRegion();

  /** LabelMapToBinaryImageFilter will produce the entire output. */
  virtual void EnlargeOutputRequestedRegion( DataObject *itkNotUsed(output) );

  virtual void GenerateData();

  virtual void ThreadedProcessLabelObject(LabelObjectType *labelObject);

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  LabelMapToBinaryImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);              //purposely not implemented

  OutputImagePixelType m_BackgroundValue;
  OutputImagePixelType m_ForegroundValue;

  typedef LabelMapToBinaryImageFilterThreader< Self > ThreaderType;
  friend class LabelMapToBinaryImageFilterThreader< Self >;
  typename ThreaderType::Pointer m_Threader;

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelMapToBinaryImageFilter.hxx"
#endif

#endif
