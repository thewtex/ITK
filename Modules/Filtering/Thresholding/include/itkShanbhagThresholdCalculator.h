
#ifndef __itkShanbhagThresholdCalculator_h
#define __itkShanbhagThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class ShanbhagThresholdCalculator
 * \brief Computes the Shanbhag threshold for an image. Aka intermeans
 *
 * Shanhbag A.G. (1994) "Utilization of Information Measure as a Means of
 * Image Thresholding" Graphical Models and Image Processing, 56(5): 414-419
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
class ITK_EXPORT ShanbhagThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef ShanbhagThresholdCalculator     Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ShanbhagThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  ShanbhagThresholdCalculator() {};
  virtual ~ShanbhagThresholdCalculator() {};
  void GenerateData(void);

private:
  ShanbhagThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkShanbhagThresholdCalculator.hxx"
#endif

#endif
