
#ifndef __itkMomentsThresholdCalculator_h
#define __itkMomentsThresholdCalculator_h

#include "itkThresholdCalculator.h"

namespace itk
{

/** \class MomentsThresholdCalculator
 * \brief Computes the Moments's threshold for an image.
 *
 *  W. Tsai, "Moment-preserving thresholding: a new approach," Computer Vision,
 * Graphics, and Image Processing, vol. 29, pp. 377-393, 1985.
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
class ITK_EXPORT MomentsThresholdCalculator : public ThresholdCalculator<THistogram, TOutput>
{
public:
  /** Standard class typedefs. */
  typedef MomentsThresholdCalculator      Self;
  typedef Object                          Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MomentsThresholdCalculator, Object);

  /** Type definition for the input image. */
  typedef THistogram  HistogramType;
  typedef TOutput     OutputType;

protected:
  MomentsThresholdCalculator() {};
  virtual ~MomentsThresholdCalculator() {};
  void GenerateData(void);

private:
  MomentsThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMomentsThresholdCalculator.hxx"
#endif

#endif
