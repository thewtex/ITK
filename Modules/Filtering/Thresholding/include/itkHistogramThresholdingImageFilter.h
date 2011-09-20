
#ifndef __itkHistogramThresholdingImageFilter_h
#define __itkHistogramThresholdingImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkFixedArray.h"

#include "itkImageToHistogramFilter.h"

#include "itkHuangThresholdCalculator.h"
#include "itkIntermodesThresholdCalculator.h"
#include "itkIsoDataThresholdCalculator.h"
#include "itkKittlerIllingworthThresholdCalculator.h"
#include "itkLiThresholdCalculator.h"
#include "itkMaximumEntropyThresholdCalculator.h"
#include "itkMomentsThresholdCalculator.h"
#include "itkOtsuThresholdCalculator.h"
#include "itkRenyiEntropyThresholdCalculator.h"
#include "itkShanbhagThresholdCalculator.h"
#include "itkThresholdCalculator.h"
#include "itkTriangleThresholdCalculator.h"
#include "itkYenThresholdCalculator.h"


namespace itk {

/** \class HistogramThresholdingImageFilter
 * \brief Threshold an image using any of the histogramming
 * methods. Default method is OTSU.
 *
 * This filter creates a binary thresholded image that separates an
 * image into foreground and background components. The filter
 * computes the threshold using any of the threshold calculators and
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
class ITK_EXPORT HistogramThresholdingImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef HistogramThresholdingImageFilter                 Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>     Superclass;
  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(HistogramThresholdingImageFilter, ImageToImageFilter);

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

  typedef enum {HUANG, INTERMODES, ISODATA, KITTLERILLINGWORTH, LI, MAXIMUMENTROPY, MOMENTS, OTSU, RENYIENTROPY, SHANBHAG, TRIANGLE, YEN} ThreshMethod;


  itkGetConstMacro(ThreshMethod, ThreshMethod);
  itkSetMacro(ThreshMethod, ThreshMethod);

  /** Set get the size of the histogram. Default is 256 */
  itkGetConstMacro(NumberOfHistogramBins, unsigned);
  itkSetMacro(NumberOfHistogramBins, unsigned);


  /** Control parameters for some methods */
  itkSetMacro( UseInterMode, bool);
  itkGetConstMacro( UseInterMode, bool );


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
  HistogramThresholdingImageFilter();
  ~HistogramThresholdingImageFilter(){};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateInputRequestedRegion();
  void GenerateData ();

private:
  HistogramThresholdingImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputPixelType      m_Threshold;
  OutputPixelType     m_InsideValue;
  OutputPixelType     m_OutsideValue;

  ThreshMethod m_ThreshMethod;

  typedef itk::Statistics::ImageToHistogramFilter<InputImageType> HistogramGeneratorType;
  typedef typename HistogramGeneratorType::HistogramType          HistogramType;

  typedef  YenThresholdCalculator<HistogramType, InputPixelType>                YenCalculatorType;
  typedef  TriangleThresholdCalculator<HistogramType, InputPixelType>           TriangleCalculatorType;
  typedef  ShanbhagThresholdCalculator<HistogramType, InputPixelType>           ShanbhagCalculatorType;
  typedef  RenyiEntropyThresholdCalculator<HistogramType, InputPixelType>       RenyiEntropyCalculatorType;
  typedef  OtsuThresholdCalculator<HistogramType, InputPixelType>               OtsuCalculatorType;
  typedef  MomentsThresholdCalculator<HistogramType, InputPixelType>            MomentsCalculatorType;
  typedef  MaximumEntropyThresholdCalculator<HistogramType, InputPixelType>     MaximumEntropyCalculatorType;
  typedef  LiThresholdCalculator<HistogramType, InputPixelType>                 LiCalculatorType;
  typedef  KittlerIllingworthThresholdCalculator<HistogramType, InputPixelType> KittlerIllingworthCalculatorType;
  typedef  IsoDataThresholdCalculator<HistogramType, InputPixelType>            IsoDataCalculatorType;
  typedef  IntermodesThresholdCalculator<HistogramType, InputPixelType>         IntermodesCalculatorType;
  typedef  HuangThresholdCalculator<HistogramType, InputPixelType>              HuangCalculatorType;


  typename YenCalculatorType::Pointer                m_YenCalc;
  typename TriangleCalculatorType::Pointer           m_TriangleCalc;
  typename ShanbhagCalculatorType::Pointer           m_ShanbhagCalc;
  typename RenyiEntropyCalculatorType::Pointer       m_RenyiEntropyCalc;
  typename OtsuCalculatorType::Pointer               m_OtsuCalc;
  typename MomentsCalculatorType::Pointer            m_MomentsCalc;
  typename MaximumEntropyCalculatorType::Pointer     m_MaximumEntropyCalc;
  typename LiCalculatorType::Pointer                 m_LiCalc;
  typename KittlerIllingworthCalculatorType::Pointer m_KittlerIllingworthCalc;
  typename IsoDataCalculatorType::Pointer            m_IsoDataCalc;
  typename IntermodesCalculatorType::Pointer         m_IntermodesCalc;
  typename HuangCalculatorType::Pointer              m_HuangCalc;

  typename HistogramGeneratorType::Pointer           m_HistogramGenerator;


  bool                m_UseInterMode;
  unsigned            m_MaximumSmoothingIterations;
  unsigned            m_NumberOfHistogramBins;
}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHistogramThresholdingImageFilter.hxx"
#endif

#endif
