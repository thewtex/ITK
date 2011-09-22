
#ifndef __itkIsoDataThresholdImageFilter_h
#define __itkIsoDataThresholdImageFilter_h

#include "itkHistogramThresholdingBaseImageFilter.h"
#include "itkIsoDataThresholdCalculator.h"

namespace itk {

/** \class IsoDataThresholdImageFilter
 * \brief Threshold an image using the IsoData Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the IsoDataThresholdCalculator and
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
class ITK_EXPORT IsoDataThresholdImageFilter :
    public HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef IsoDataThresholdImageFilter                      Self;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;


  typedef HistogramThresholdingBaseImageFilter<TInputImage,TOutputImage>     Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(IsoDataThresholdImageFilter, HistogramThresholdingBaseImageFilter);

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
  typedef IsoDataThresholdCalculator<typename Superclass::HistogramType, InputPixelType> CalculatorType;

  IsoDataThresholdImageFilter()
    {
    this->m_Calculator = CalculatorType::New();
    }

  ~IsoDataThresholdImageFilter(){};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << "IsoData thresholding" << std::endl;
  }

private:
  IsoDataThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


}; // end of class

} // end namespace itk

#endif
