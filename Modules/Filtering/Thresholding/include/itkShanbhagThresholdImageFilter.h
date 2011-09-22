
#ifndef __itkShanbhagThresholdImageFilter_h
#define __itkShanbhagThresholdImageFilter_h

#include "itkHistogramThresholdingBaseImageFilter.h"
#include "itkShanbhagThresholdCalculator.h"

namespace itk {

/** \class ShanbhagThresholdImageFilter
 * \brief Threshold an image using the Shanbhag Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the ShanbhagThresholdCalculator and
 * applies that theshold to the input image using the
 * BinaryThresholdImageFilter.
 *
 * \author Richard Beare
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/10380/3279  or
 * http://www.insight-journal.org/browse/publication/811
 *
 * \ingroup Multithreaded
 * \ingroup ITKThresholding
 */

template<class TInputImage, class TOutputImage>
class ITK_EXPORT ShanbhagThresholdImageFilter :
    public HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef ShanbhagThresholdImageFilter                     Self;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;


  typedef HistogramThresholdingBaseImageFilter<TInputImage,TOutputImage>     Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ShanbhagThresholdImageFilter, HistogramThresholdingBaseImageFilter);

  typedef TInputImage                       InputImageType;
  typedef TOutputImage                      OutputImageType;

  /** Image pixel value typedef. */
  typedef typename InputImageType::PixelType   InputPixelType;
  typedef typename OutputImageType::PixelType  OutputPixelType;

  /** Image related typedefs. */
  typedef typename InputImageType::Pointer  InputImagePointer;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  typedef typename InputImageType::SizeType    InputSizeType;
  typedef typename InputImageType::IndexType   InputIndexType;
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename OutputImageType::SizeType   OutputSizeType;
  typedef typename OutputImageType::IndexType  OutputIndexType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;


  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputEqualityComparableCheck,
    (Concept::EqualityComparable<OutputPixelType>));
  itkConceptMacro(InputOStreamWritableCheck,
    (Concept::OStreamWritable<InputPixelType>));
  itkConceptMacro(OutputOStreamWritableCheck,
    (Concept::OStreamWritable<OutputPixelType>));
  /** End concept checking */
#endif
protected:

  typedef ShanbhagThresholdCalculator<typename Superclass::HistogramType, InputPixelType> CalculatorType;

  ShanbhagThresholdImageFilter()
    {
    this->m_Calculator = CalculatorType::New();
    }

  ~ShanbhagThresholdImageFilter(){};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
  Superclass::PrintSelf(os,indent);
  os << indent << "Shanbhag thresholding" << std::endl;
  }

private:
  ShanbhagThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk

#endif
