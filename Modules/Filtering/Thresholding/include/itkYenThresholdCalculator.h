
#ifndef __itkYenThresholdCalculator_h
#define __itkYenThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class YenThresholdCalculator
 * \brief Computes the Yen's threshold for an image.
 *
 * Implements Yen  thresholding method
 * 1) Yen J.C., Chang F.J., and Chang S. (1995) "A New Criterion
 *    for Automatic Multilevel Thresholding" IEEE Trans. on Image
 *    Processing, 4(3): 370-378
 * 2) Sezgin M. and Sankur B. (2004) "Survey over Image Thresholding
 *    Techniques and Quantitative Performance Evaluation" Journal of
 *    Electronic Imaging, 13(1): 146-165
 *    http://citeseer.ist.psu.edu/sezgin04survey.html
 *
 * M. Emre Celebi
 * 06.15.2007
 *
 * This class is templated over the input histogram type.
 * \warning This calculator assumes that the input histogram has only one dimension.
 *
 * \author Richard Beare
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/10380/3279  or
 * http://www.insight-journal.org/browse/publication/811
 *
 * \ingroup Operators
 * \ingroup ITKThresholding
 */
template <class THistogram, class TOutput=double>
class ITK_EXPORT YenThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef YenThresholdCalculator          Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(YenThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  YenThresholdCalculator() {};
  virtual ~YenThresholdCalculator() {};
  void GenerateData(void);

private:
  YenThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkYenThresholdCalculator.hxx"
#endif

#endif
