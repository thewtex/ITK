
#ifndef __itkIntermodesThresholdImageFilter_h
#define __itkIntermodesThresholdImageFilter_h

#include "itkHistogramThresholdingBaseImageFilter.h"
#include "itkIntermodesThresholdCalculator.h"


namespace itk {

/** \class IntermodesThresholdImageFilter
 * \brief Threshold an image using the Intermodes Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the IntermodesThresholdCalculator and
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

template<class TInputImage, class TOutputImage=double>
class ITK_EXPORT IntermodesThresholdImageFilter :
    public HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef IntermodesThresholdImageFilter                   Self;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;


  typedef HistogramThresholdingBaseImageFilter<TInputImage,TOutputImage>     Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(IntermodesThresholdImageFilter, HistogramThresholdingBaseImageFilter);

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

  void SetMaximumSmoothingIterations(unsigned long I)
  {
    this->m_MyCalc->SetMaximumSmoothingIterations(I);
  }

  const unsigned long & GetMaximumSmoothingIterations()
  {
    return(this->m_MyCalc->GetMaximumSmoothingIterations());
  }

  /** select whether midpoint (intermode=true) or minimum between
  peaks is used */
  void SetUseInterMode(bool V)
  {
    this->m_MyCalc->SetUseInterMode(V);
  }

  bool GetUseInterMode()
  {
    return(this->m_MyCalc->GetUseInterMode());
  }

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
  typedef IntermodesThresholdCalculator<typename Superclass::HistogramType, InputPixelType> CalculatorType;

  // this calculator has an extended interface, so keep this copy.
  typename CalculatorType::Pointer m_MyCalc;

  IntermodesThresholdImageFilter()
    {
    m_MyCalc = CalculatorType::New();
    this->m_Calculator = m_MyCalc;
    }

  ~IntermodesThresholdImageFilter(){};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << "Intermodes thresholding" << std::endl;
    os << indent << "Smoothing  " << m_MyCalc->GetMaximumSmoothingIterations() << std::endl;
    os << indent << "Intermodes " << m_MyCalc->GetUseInterMode() << std::endl;
  }

private:
  IntermodesThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk

#endif
