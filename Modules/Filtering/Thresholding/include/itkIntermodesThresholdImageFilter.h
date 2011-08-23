
#ifndef __itkIntermodesThresholdImageFilter_h
#define __itkIntermodesThresholdImageFilter_h

#include "itkHistogramThresholdImageFilter.h"
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

template<class TInputImage, class TOutputImage>
class ITK_EXPORT IntermodesThresholdImageFilter :
    public HistogramThresholdImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef IntermodesThresholdImageFilter                              Self;
  typedef HistogramThresholdImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(IntermodesThresholdImageFilter, ImageToImageFilter);

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

  typedef typename Superclass::HistogramType                             HistogramType;
  typedef IntermodesThresholdCalculator< HistogramType, InputPixelType > CalculatorType;

  itkSetMacro( MaximumSmoothingIterations, unsigned long);
  itkGetConstMacro( MaximumSmoothingIterations, unsigned long );

  /** select whether midpoint (intermode=true) or minimum between
     peaks is used */
  itkSetMacro( UseInterMode, bool);
  itkGetConstMacro( UseInterMode, bool );

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

protected:
  IntermodesThresholdImageFilter()
    {
    this->SetCalculator( CalculatorType::New() );
    m_MaximumSmoothingIterations = 10000;
    m_UseInterMode = true;
    }
  ~IntermodesThresholdImageFilter(){};

  void ConfigureCalculator()
    {
    CalculatorType * calculator = dynamic_cast<CalculatorType *>(this->GetCalculator());
    if( calculator != NULL )
      {
      calculator->SetMaximumSmoothingIterations( m_MaximumSmoothingIterations );
      calculator->SetUseInterMode( m_UseInterMode );
      }
    }

  void PrintSelf(std::ostream & os, Indent indent) const
    {
    Superclass::PrintSelf(os, indent);
    os << indent << "MaximumSmoothingIterations: " << m_MaximumSmoothingIterations << std::endl;
    os << indent << "UseInterMode: " << m_UseInterMode << std::endl;
    }

private:
  IntermodesThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  unsigned            m_MaximumSmoothingIterations;
  bool                m_UseInterMode;

}; // end of class

} // end namespace itk

#endif
