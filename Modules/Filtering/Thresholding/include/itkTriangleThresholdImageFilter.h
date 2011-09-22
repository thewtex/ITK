
#ifndef __itkTriangleThresholdImageFilter_h
#define __itkTriangleThresholdImageFilter_h

#include "itkHistogramThresholdingBaseImageFilter.h"
#include "itkTriangleThresholdCalculator.h"

namespace itk {

/** \class TriangleThresholdImageFilter
 * \brief Threshold an image using the Triangle Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the TriangleThresholdCalculator and
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
class ITK_EXPORT TriangleThresholdImageFilter :
    public HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef TriangleThresholdImageFilter                     Self;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;


  typedef HistogramThresholdingBaseImageFilter<TInputImage,TOutputImage>     Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(TriangleThresholdImageFilter, HistogramThresholdingBaseImageFilter);

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

  typedef TriangleThresholdCalculator<typename Superclass::HistogramType, InputPixelType> CalculatorType;

  TriangleThresholdImageFilter()
    {
    this->m_Calculator = CalculatorType::New();
    }

  ~TriangleThresholdImageFilter(){};
  void PrintSelf(std::ostream& os, Indent indent) const
  {
  Superclass::PrintSelf(os,indent);
  os << indent << "Triangle thresholding" << std::endl;

  }

private:
  TriangleThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk

#endif
