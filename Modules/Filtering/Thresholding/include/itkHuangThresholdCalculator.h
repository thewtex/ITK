
#ifndef __itkHuangThresholdCalculator_h
#define __itkHuangThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class HuangThresholdImageCalculator
 * \brief Computes the Huang's threshold for an image.
 *
 * This calculator computes the Huang's fuzzy threshold which separates an image
 * into foreground and background components. Uses Shannon's entropy
 * function (one can also use Yager's entropy function)
 * Huang L.-K. and Wang M.-J.J. (1995) "Image Thresholding by Minimizing
 * the Measures of Fuzziness" Pattern Recognition, 28(1): 41-51
 * Reimplemented (to handle 16-bit efficiently) by Johannes Schindelin Jan 31, 2011
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
class ITK_EXPORT HuangThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef HuangThresholdCalculator        Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(HuangThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  HuangThresholdCalculator() {};
  virtual ~HuangThresholdCalculator() {};
  void GenerateData(void);

private:
  HuangThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHuangThresholdCalculator.hxx"
#endif

#endif
