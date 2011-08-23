
#ifndef __itkMaximumEntropyThresholdImageFilter_h
#define __itkMaximumEntropyThresholdImageFilter_h

#include "itkHistogramThresholdImageFilter.h"
#include "itkMaximumEntropyThresholdCalculator.h"

namespace itk {

/** \class MaximumEntropyThresholdImageFilter
 * \brief Threshold an image using the MaximumEntropy Threshold
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using the MaximumEntropyThresholdCalculator and
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
class ITK_EXPORT MaximumEntropyThresholdImageFilter :
    public HistogramThresholdImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef MaximumEntropyThresholdImageFilter                                   Self;
  typedef HistogramThresholdImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(MaximumEntropyThresholdImageFilter, ImageToImageFilter);

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

  typedef typename Superclass::HistogramType                        HistogramType;
  typedef MaximumEntropyThresholdCalculator< HistogramType, InputPixelType > CalculatorType;

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

protected:
  MaximumEntropyThresholdImageFilter()
    {
    this->SetCalculator( CalculatorType::New() );
    }
  ~MaximumEntropyThresholdImageFilter(){};

private:
  MaximumEntropyThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
}; // end of class

} // end namespace itk

#endif
