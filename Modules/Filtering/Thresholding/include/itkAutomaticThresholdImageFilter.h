
#ifndef __itkAutomaticThresholdImageFilter_h
#define __itkAutomaticThresholdImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkHistogram.h"
#include "itkThresholdCalculator.h"

namespace itk {

/** \class AutomaticThresholdImageFilter
 * \brief Threshold an image using a ThresholdCalculator
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using a user provided ThresholdCalculator and
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
class ITK_EXPORT AutomaticThresholdImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef AutomaticThresholdImageFilter                    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(AutomaticThresholdImageFilter, ImageToImageFilter);

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

   typedef typename NumericTraits< InputPixelType >::ValueType ValueType;
   typedef typename NumericTraits< ValueType >::RealType       ValueRealType;
   typedef Statistics::Histogram< ValueRealType >              HistogramType;
   typedef typename HistogramType::Pointer                     HistogramPointer;
   typedef typename HistogramType::ConstPointer                HistogramConstPointer;
   typedef typename HistogramType::SizeType                    HistogramSizeType;
   typedef typename HistogramType::MeasurementType             HistogramMeasurementType;
   typedef typename HistogramType::MeasurementVectorType       HistogramMeasurementVectorType;
   typedef ThresholdCalculator<HistogramType, InputPixelType>  CalculatorType;
   typedef typename CalculatorType::Pointer                    CalculatorPointer;

  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      OutputImageType::ImageDimension );

  /** Set the "outside" pixel value. The default value
   * NumericTraits<OutputPixelType>::Zero. */
  itkSetMacro(OutsideValue,OutputPixelType);

  /** Get the "outside" pixel value. */
  itkGetConstMacro(OutsideValue,OutputPixelType);

  /** Set the "inside" pixel value. The default value
   * NumericTraits<OutputPixelType>::max() */
  itkSetMacro(InsideValue,OutputPixelType);

  /** Get the "inside" pixel value. */
  itkGetConstMacro(InsideValue,OutputPixelType);

  /** Get the computed threshold. */
  itkGetConstMacro(Threshold,InputPixelType);

  /** Set/Get the calculator to use to compute the threshold */
  itkSetObjectMacro(Calculator, CalculatorType);
  itkGetObjectMacro(Calculator, CalculatorType);

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
  AutomaticThresholdImageFilter();
  ~AutomaticThresholdImageFilter(){};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateInputRequestedRegion();
  void GenerateData ();

  /** This method should be overiden in subclasses to pass extra parameters
   * to the calculator */
  virtual void ConfigureCalculator() {}

private:
  AutomaticThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OutputPixelType     m_InsideValue;
  OutputPixelType     m_OutsideValue;
  InputPixelType      m_Threshold;
  CalculatorPointer   m_Calculator;

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAutomaticThresholdImageFilter.hxx"
#endif

#endif
