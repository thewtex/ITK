
#ifndef __itkMaximumEntropyThresholdCalculator_h
#define __itkMaximumEntropyThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class MaximumEntropyThresholdCalculator
 * \brief Computes the MaximumEntropy's threshold for an image.
 *
 * Implements Kapur-Sahoo-Wong (Maximum Entropy) thresholding method
 * Kapur J.N., Sahoo P.K., and Wong A.K.C. (1985) "A New Method for
 * Gray-Level Picture Thresholding Using the Entropy of the Histogram"
 * Graphical Models and Image Processing, 29(3): 273-285
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
class ITK_EXPORT MaximumEntropyThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef MaximumEntropyThresholdCalculator      Self;
  typedef Object                                 Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaximumEntropyThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  MaximumEntropyThresholdCalculator() {};
  virtual ~MaximumEntropyThresholdCalculator() {};
  void GenerateData(void);

private:
  MaximumEntropyThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaximumEntropyThresholdCalculator.hxx"
#endif

#endif
