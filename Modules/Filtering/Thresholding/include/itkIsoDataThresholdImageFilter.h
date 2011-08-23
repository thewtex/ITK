
#ifndef __itkIsoDataThresholdImageFilter_h
#define __itkIsoDataThresholdImageFilter_h

#include "itkAutomaticThresholdImageFilter.h"
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

template<class TInputImage, class TOutputImage=double>
class ITK_EXPORT IsoDataThresholdImageFilter :
    public AutomaticThresholdImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef IsoDataThresholdImageFilter                                 Self;
  typedef AutomaticThresholdImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef SmartPointer<const Self>                                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(IsoDataThresholdImageFilter, ImageToImageFilter);

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

  typedef typename Superclass::HistogramType                          HistogramType;
  typedef IsoDataThresholdCalculator< HistogramType, InputPixelType > CalculatorType;

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

protected:
  IsoDataThresholdImageFilter()
    {
    this->SetCalculator( CalculatorType::New() );
    }
  ~IsoDataThresholdImageFilter(){};

private:
  IsoDataThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
}; // end of class

} // end namespace itk

#endif
